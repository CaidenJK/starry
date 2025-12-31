#pragma once 

#include <mutex>
#include <optional>

namespace StarryManager
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
            if (!requestPointer) return ResourceRequest::STALE;
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

        [[nodiscard]] ResourceRequest::ResourceState wait() {
            volatile ResourceRequest::ResourceState state;
            do {
                state = request();
            }
            while (state == ResourceRequest::ResourceState::NO);

            return state;
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
}