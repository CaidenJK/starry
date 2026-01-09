#pragma once 

#include <mutex>
#include <optional>
#include <vector>

namespace StarryManager
{
    class AssetManager;
    
    // Core Request
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

        const std::string resourceID;
        const std::vector<size_t> resourceArgs = {};

        std::recursive_mutex mutex;
        ResourceState resourceState = NO;

        void* resource = nullptr;

        const size_t callerUUID;
        const size_t senderUUID;
        
        private:
            ResourceRequest(size_t callerUUID, size_t senderUUID, std::string resourceID, std::vector<size_t>& resourceArgs);

            static std::shared_ptr<ResourceRequest> create(size_t caller, size_t sender, std::string id, std::vector<size_t>& args);
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
            if (requestPointer && requestPointer.use_count() == 2) {
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

        std::shared_ptr<ResourceRequest> requestPointer = nullptr;
        T* resourcePointer = nullptr;
    };

    // Sender Side API
    class ResourceAsk
    {
        public:
            ResourceAsk(std::shared_ptr<ResourceRequest> ref) : requestPointer(ref) {}
            ~ResourceAsk();

            void setResource(void* resource);
            ResourceRequest::ResourceState getState();
            
            std::string getID();
            std::vector<size_t> getArguments();

            void invalidate();

        private:
            std::shared_ptr<ResourceRequest> requestPointer = nullptr;
    };
}