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
            static void InitManager(const std::string& package);

            // Note references are always valid since they are unregistered on asset destruction
		    void registerAsset(StarryAsset* asset);
		    void unregisterAsset(size_t uuid);
		    void updateAssetPointer(size_t uuid, StarryAsset* newPtr);

            void dumpRegisteredAssets(bool names);
            void registerAssetAlert(size_t uuid);

            bool isFatal() { return hasFatal.load(); }

            void setFileLogging(bool value) { logger->setFileLogging(value); }
            void setExitRights(bool value) { hasExitRights.store(value); }

            template <typename T>
            ResourceHandle<T> requestResource(size_t callerID, size_t senderID, std::string resourceName, std::vector<size_t> resourceArgs)
            {
                std::scoped_lock lock(resourceMutex);
                registeryMutex.lock();
                StarryAsset* asset;

                if (senderID == FILE_REQUEST) {
                    asset = (StarryAsset*)fileHandler;
                }
                else {
                    auto iter = registeredAssets.find(senderID);
                    if (iter == registeredAssets.end()) {
                        Alert("No asset has given UUID.", CRITICAL); 
                        return {};
                    }
                    asset = iter->second;
                }
                registeryMutex.unlock();

                resourceRequests.emplace(ResourceRequest::create(callerID, asset->getUUID(), resourceName, resourceArgs));
                resourceCV.notify_all();
                
                return ResourceHandle<T>(resourceRequests.back());
            }

            template <typename T>
            ResourceHandle<T> requestResource(size_t callerID, std::string senderName, std::string resourceName, std::vector<size_t> resourceArgs)
            {
                std::scoped_lock lock(resourceMutex);
                registeryMutex.lock();
                for(auto& it : registeredAssets) {
                    if (it.second->getAssetName().compare(senderName) == 0) {
                        size_t uuid = it.second->getUUID();
                        registeryMutex.unlock();

                        resourceRequests.emplace(ResourceRequest::create(callerID, uuid, resourceName, resourceArgs));
						resourceCV.notify_all();

                        return ResourceHandle<T>(resourceRequests.back());
                    }
                }

                registeryMutex.unlock();
                Alert(std::string("No sender of resource has given name: ") + senderName, WARNING);
                return {};
            }

            ASSET_NAME("Asset Manager")

        private:
            AssetManager(const std::string& name);
            static std::shared_ptr<AssetManager> globalPointer;

            void submitAsk(std::shared_ptr<ResourceRequest>& request);

            std::atomic<bool> hasExitRights = false;
            std::string packageName = "N/A";

            Logger* logger;
            FileHandler* fileHandler;

            std::recursive_mutex registeryMutex;
            std::unordered_map<size_t, StarryAsset*> registeredAssets;
            
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
	ResourceHandle<T> StarryAsset::requestResource(size_t senderID, std::string resourceName, std::vector<size_t> resourceArgs)
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