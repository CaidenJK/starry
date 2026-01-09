#include "Descriptor.h"

#define ERROR_VOLATILE(x) x; if (getAlertSeverity() == FATAL) { return; }

#include "Device.h"

#include <array>

namespace StarryRender
{
    Descriptor::Descriptor()
    {
    }

    Descriptor::~Descriptor()
    {
		destroy();
    }

	void Descriptor::init(uint64_t deviceUUID)
	{
		device = requestResource<Device>(deviceUUID, "self");

		createDescriptorSetLayout();
		createDescriptorPool();
	}

	void Descriptor::destroy()
	{
		if (device) {
			vkDestroyDescriptorPool((*device).getDevice(), descriptorPool, nullptr);
			vkDestroyDescriptorSetLayout((*device).getDevice(), descriptorSetLayout, nullptr);
		}
	}

    void Descriptor::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
		layoutInfo.pBindings = bindings.data();
		
		if (device.wait() != ResourceState::YES) {
			Alert("Device died before it was ready to be used.", FATAL);
			return;
		}
		if (vkCreateDescriptorSetLayout((*device).getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			Alert("Failed to create descriptor set layout!", FATAL);
		}
	}

    void Descriptor::createDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

		if (device.wait() != ResourceState::YES) {
			Alert("Device died before it was ready to be used.", FATAL);
			return;
		}
		if (vkCreateDescriptorPool((*device).getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			Alert("Failed to create descriptor pool!", FATAL);
			return;
		}
	}

    void Descriptor::createDescriptorSets(uint64_t ubUUID, uint64_t txUUID)
    {
       /* auto uniformBuffers = requestResource<std::vector<VkBuffer>>(uniformBufferUUID, "VkBuffers");
        auto textureImageView = requestResource<VkImageView>(imageTextureID, "Image View");
        auto textureSampler = requestResource<VkSampler>(imageTextureID, "Sampler");

        if (uniformBuffers.wait() != ResourceState::YES || 
            textureImageView.wait() != ResourceState::YES || 
            textureSampler.wait() != ResourceState::YES) {
            Alert("Supplied resources died before they were ready for use.", FATAL);
            return;
        } */

		auto ub = requestResource<UniformBuffer*>(ubUUID, "self");
		auto tx = requestResource<TextureImage*>(txUUID, "self");

		if (descriptorPool == VK_NULL_HANDLE || descriptorSetLayout == VK_NULL_HANDLE) {
			Alert("Descriptor pool or set layout not ready before allocating descriptor sets.", FATAL);
			return;
		}

		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		if (device.wait() != ResourceState::YES) {
			Alert("Device died before it was ready to be used.", FATAL);
			return;
		}
		if (vkAllocateDescriptorSets((*device).getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			Alert("Failed to allocate descriptor sets!", FATAL);
			return;
		}

		if (ub.wait() != ResourceState::YES || tx.wait() != ResourceState::YES) {
			Alert("Invalid buffers passed to descriptor!", FATAL);
			return;
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			auto bufferInfo = (*ub)->getDescriptorInfo(i);
			auto imageInfo = (*tx)->getDescriptorInfo(i);

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr; // Optional
			descriptorWrites[0].pTexelBufferView = nullptr; // Optional

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets((*device).getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
    }
}