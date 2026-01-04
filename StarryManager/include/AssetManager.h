#pragma once

#include "StarryAsset.h"
#include "Logger.h"
#include "Resource.h"
#include "FileHandler.h"

#include <optional>
#include <unordered_map>
#include <condition_variable>

namespace StarryManager
{
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
            void setExitRights(bool value) { hasExitRights.store(value); }

            template <typename T>
            ResourceHandle<T> requestResource(uint64_t callerID, uint64_t senderID, std::string resourceName, std::vector<size_t> resourceArgs)
            {
                std::scoped_lock lock(resourceMutex);
                registeryMutex.lock();
                size_t resourceID = INVALID_RESOURCE; 
                StarryAsset* asset;
                if (senderID == FILE_REQUEST) {
                    asset = (StarryAsset*)fileHandler;
                }
                else {
                    auto iter = registeredAssets.find(senderID);
                    if (iter == registeredAssets.end()) {
                        registerAlert("No asset has given UUID.", CRITICAL); 
                        return {};
                    }
                    asset = iter->second;
                }
                resourceID = asset->getResourceIDFromString(resourceName);
                registeryMutex.unlock();

                resourceRequests.emplace(ResourceRequest::create(callerID, asset->getUUID(), resourceID, resourceArgs));
                if (resourceID == INVALID_RESOURCE) resourceRequests.back()->resourceState = ResourceRequest::ResourceState::DEAD;
                resourceCV.notify_all();
                
                return ResourceHandle<T>(resourceRequests.back());
            }

            template <typename T>
            ResourceHandle<T> requestResource(uint64_t callerID, std::string senderName, std::string resourceName, std::vector<size_t> resourceArgs)
            {
                std::scoped_lock lock(resourceMutex);
                registeryMutex.lock();
                for(auto& it : registeredAssets) {
                    if (it.second->getAssetName().compare(senderName) == 0) {
                        uint64_t uuid = it.second->getUUID();
                        size_t resourceID = it.second->getResourceIDFromString(resourceName);
                        registeryMutex.unlock();

                        resourceRequests.emplace(ResourceRequest::create(callerID, uuid, resourceID, resourceArgs));
                        if (resourceID == INVALID_RESOURCE) resourceRequests.back()->resourceState = ResourceRequest::ResourceState::DEAD;
						resourceCV.notify_all();

                        return ResourceHandle<T>(resourceRequests.back());
                    }
                }

                registeryMutex.unlock();
                registerAlert(std::string("No sender of resource has given name: ") + senderName, WARNING);
                return {};
            }

            ASSET_NAME("Asset Manager")

        private:
            AssetManager();
            static std::shared_ptr<AssetManager> globalPointer;

            void findResources(std::shared_ptr<ResourceRequest>& request);

            std::atomic<bool> hasExitRights = false;

            Logger* logger;
            FileHandler* fileHandler;

            std::recursive_mutex registeryMutex;
            std::unordered_map<uint64_t, StarryAsset*> registeredAssets;
            
            std::mutex resourceMutex;
            std::queue<std::shared_ptr<ResourceRequest>> resourceRequests;
            std::vector<std::shared_ptr<ResourceRequest>> closedRequests;

			std::condition_variable resourceCV;
            std::thread assetThread;
            void worker();

            std::atomic<bool> hasFatal = false;
            static std::atomic<bool> isDead;

            Logger::AssetCall getFatalAlert();
            Logger::AssetCall getVersionAlert();
    };

    template <typename T> 
	ResourceHandle<T> StarryAsset::requestResource(uint64_t senderID, std::string resourceName, std::vector<size_t> resourceArgs)
    {
        if (auto ptr = AssetManager::get().lock()) return ptr->requestResource<T>(uuid, senderID, resourceName, resourceArgs);
        return {};
    }

	template <typename T> 
	ResourceHandle<T> StarryAsset::requestResource(std::string senderName, std::string resourceName, std::vector<size_t> resourceArgs)
    {
        if (auto ptr = AssetManager::get().lock()) return ptr->requestResource<T>(uuid, senderName, resourceName, resourceArgs);
        return {};
    }
}
// manage shared resources
// logger is enclosed class