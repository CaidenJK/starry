#pragma once

#include "StarryAsset.h"
#include "Logger.h"

#include <optional>

namespace StarryLog
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
    };

    // Caller side API
    template<typename T>
    class ResourceHandle : public StarryAsset
    {
        friend class AssetManager;
    public:
        ResourceHandle() = default; // empty handle
        ~ResourceHandle() {
            std::scoped_lock lock(requestPointer->mutex);
            if (requestPointer) requestPointer->resourceState = ResourceRequest::STALE;
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
                    registerAlert("Requested resource is NULL. Resource is either dead/stale or not type compatible.", CRITICAL);
                    requestPointer->resourceState = ResourceRequest::STALE;
                    return ResourceRequest::STALE;
                }
            }

            return state;
        }

    private:
        ResourceHandle(std::shared_ptr<ResourceRequest> reference) : requestPointer(reference) {}

        std::shared_ptr<ResourceRequest> requestPointer;
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
            ResourceHandle<T> requestResource(uint64_t callerID, uint64_t senderID, uint64_t resourceID)
            {
                std::scoped_lock lock(resourceMutex);
                resourceRequests.emplace(std::make_shared<ResourceRequest>(callerID, senderID, resourceID));
                
                return ResourceHandle<T>(resourceRequests.back());
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
    // returns uuid, can get a response using it

    template <typename T>
	ResourceHandle<T> StarryAsset::requestResource(uint64_t senderID, uint64_t resourceID)
	{
		if (auto ptr = AssetManager::get().lock()) return ptr->requestResource<T>(uuid, senderID, resourceID);
		return {};
	}
}
// manage shared resources
// logger is enclosed class
// manage exit state