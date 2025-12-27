#pragma once

#include "StarryAsset.h"
#include "Logger.h"

#include <optional>
#include <unordered_map>
#include <typeindex>

#include <iostream>

namespace StarryAssets
{
    class AssetManager;
    
    // Sender side API
    struct ResourceRequest
    {
        friend class AssetManager;
        
        enum ResourceState
        {
            YES, // success
            NO, // waiting
            DEAD, // termination for sender
            STALE // termination for caller
        };

        const uint64_t resourceID; 

        std::mutex mutex;
        ResourceState resourceState = NO;
        void* resource = nullptr;

        const uint64_t callerUUID; // Might not need
        const uint64_t senderUUID;
        
        private:
            ResourceRequest(uint64_t callerUUID, uint64_t senderUUID, size_t resourceID);

            static std::shared_ptr<ResourceRequest> create(uint64_t caller, uint64_t sender, size_t id);
    };

    // Caller side API
    template<typename T>
    class ResourceHandle
    {
        friend class AssetManager;
    public:
        ResourceHandle() = default; // empty handle
        ~ResourceHandle()
        {
            if (requestPointer && requestPointer.use_count() == 1) {
                std::scoped_lock lock(requestPointer->mutex);
                requestPointer->resourceState = ResourceRequest::STALE;
            }
        }

        ResourceHandle(const ResourceHandle& handle) noexcept
        {
            requestPointer = handle.requestPointer;
            resourcePointer = handle.resourcePointer;
        }

        ResourceHandle& operator=(const ResourceHandle& handle) noexcept
        {
            requestPointer = handle.requestPointer;
            resourcePointer = handle.resourcePointer;

            return *this;
        }

        std::optional<T*> get()
        {
            std::scoped_lock lock(requestPointer->mutex);
            if (requestPointer->resourceState == ResourceRequest::YES) return resourcePointer;
            return {};
        }

        ResourceRequest::ResourceState request()
        {
            std::scoped_lock lock(requestPointer->mutex);
            auto state = requestPointer->resourceState;
            if (state == ResourceRequest::YES && resourcePointer == nullptr) {
                resourcePointer = static_cast<T*>(requestPointer->resource); // Need type checking

                if (resourcePointer == nullptr) {
                    requestPointer->resourceState = ResourceRequest::STALE;
                    return ResourceRequest::STALE;
                }
            }
            return state;
        }

        bool hasRequest()
        {
            return request() == ResourceRequest::ResourceState::YES;
        }

        void wait() {
            volatile bool isReady = false;
            while (!isReady) isReady = hasRequest();

            return;
        }

        T& operator*()
        {
            return *(get().value());
        }

        explicit operator bool() {
            return hasRequest();
        }

    private:
        ResourceHandle(std::shared_ptr<ResourceRequest> reference) : requestPointer(reference) {}

        std::shared_ptr<ResourceRequest> requestPointer = {};
        T* resourcePointer = nullptr;
    };

    class AssetManager : public StarryAsset
    {
        public:
            ~AssetManager();
            static std::weak_ptr<AssetManager> get();

            // Note references are always valid since they are unregistered on asset destruction
		    void registerAsset(StarryAsset* asset);
		    void unregisterAsset(uint64_t uuid);
		    void updateAssetPointer(uint64_t uuid, StarryAsset* newPtr);

            void dumpRegisteredAssets(bool names);
            void registerAssetAlert(uint64_t uuid);

            bool isFatal() { return hasFatal.load(); }

            void setFileLogging(bool value) { logger->setFileLogging(value); }

            template <typename T>
            ResourceHandle<T> requestResource(uint64_t callerID, uint64_t senderID, size_t resourceID)
            {
                std::scoped_lock lock(resourceMutex);
                resourceRequests.emplace(ResourceRequest::create(callerID, senderID, resourceID));
                
                return ResourceHandle<T>(resourceRequests.back());
            }

            template <typename T>
            ResourceHandle<T> requestResource(uint64_t callerID, std::string senderName, size_t resourceID)
            {
                std::scoped_lock lock(resourceMutex);
                registeryMutex.lock();
                for(auto& it : registeredAssets) {
                    if (it.second->getAssetName().compare(senderName) == 0) {
                        registeryMutex.unlock();
                        resourceRequests.emplace(ResourceRequest::create(callerID, it.second->getUUID(), resourceID));
                        return ResourceHandle<T>(resourceRequests.back());
                    }
                }
                registeryMutex.unlock();
                registerAlert(std::string("No sender of resource has given name: ") + senderName, WARNING);
                return {};
            }

            template <typename T>
            ResourceHandle<T> requestResource(uint64_t callerID, uint64_t senderID, std::string resourceName)
            {
                std::scoped_lock lock(resourceMutex);
                registeryMutex.lock();
                size_t resourceID = 0; auto asset = registeredAssets.find(senderID);
                if (asset == registeredAssets.end()) size_t resourceID = asset->second->getResourceIDFromString(resourceName);
                registeryMutex.unlock();
                resourceRequests.emplace(ResourceRequest::create(callerID, senderID, resourceID));
                
                return ResourceHandle<T>(resourceRequests.back());
            }

            template <typename T>
            ResourceHandle<T> requestResource(uint64_t callerID, std::string senderName, std::string resourceName)
            {
                std::scoped_lock lock(resourceMutex);
                registeryMutex.lock();
                for(auto& it : registeredAssets) {
                    if (it.second->getAssetName().compare(senderName) == 0) {
                        uint64_t uuid = it.second->getUUID();
                        size_t resourceID = it.second->getResourceIDFromString(resourceName);
                        registeryMutex.unlock();
                        resourceRequests.emplace(ResourceRequest::create(callerID, uuid, resourceID));
                        return ResourceHandle<T>(resourceRequests.back());
                    }
                }
                registeryMutex.unlock();
                registerAlert(std::string("No sender of resource has given name: ") + senderName, WARNING);
                return {};
            }

            const std::string getAssetName() override {return "Asset Manager";}
        private:
            AssetManager();
            static std::shared_ptr<AssetManager> globalPointer;

            void findResources(std::shared_ptr<ResourceRequest>& request);

            Logger* logger;

            std::mutex registeryMutex;
            std::unordered_map<uint64_t, StarryAsset*> registeredAssets;
            
            std::mutex resourceMutex;
            std::queue<std::shared_ptr<ResourceRequest>> resourceRequests;
            std::vector<std::shared_ptr<ResourceRequest>> closedRequests;

            std::thread assetThread;
            void worker();

            std::atomic<bool> hasFatal = false;
            static std::atomic<bool> isDead;
    };

    template <typename T>
	ResourceHandle<T> StarryAsset::requestResource(uint64_t senderID, size_t resourceID)
	{
		if (auto ptr = AssetManager::get().lock()) return std::move(ptr->requestResource<T>(uuid, senderID, resourceID));
		return {};
	}

    template <typename T>
	ResourceHandle<T> StarryAsset::requestResource(uint64_t senderID, std::string resourceName)
	{
		if (auto ptr = AssetManager::get().lock()) return ptr->requestResource<T>(uuid, senderID, resourceName);
		return {};
	}

    template <typename T>
    ResourceHandle<T> StarryAsset::requestResource(std::string senderName, size_t resourceID)
	{
		if (auto ptr = AssetManager::get().lock()) return ptr->requestResource<T>(uuid, senderName, resourceID);
		return {};
	}

    template <typename T>
    ResourceHandle<T> StarryAsset::requestResource(std::string senderName, std::string resourceName)
	{
		if (auto ptr = AssetManager::get().lock()) return ptr->requestResource<T>(uuid, senderName, resourceName);
		return {};
	}
}
// manage shared resources
// logger is enclosed class