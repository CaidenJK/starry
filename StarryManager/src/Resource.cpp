#include "Resource.h"

namespace StarryManager
{
    ResourceRequest::ResourceRequest(size_t callerUUID, size_t senderUUID, std::string resourceID, std::vector<size_t>& resourceArgs) :
    callerUUID(callerUUID), senderUUID(senderUUID), resourceID(resourceID), resourceArgs(resourceArgs)
    {
    }

    std::shared_ptr<ResourceRequest> ResourceRequest::create(size_t caller, size_t sender, std::string id, std::vector<size_t>& args)
    {
        return std::shared_ptr<ResourceRequest>(new ResourceRequest(caller, sender, id, args));
    }

    ResourceAsk::~ResourceAsk()
    {
        invalidate();
    }

    void ResourceAsk::setResource(void* resource)
    {
        std::scoped_lock lock(RequestPointer->mutex);
        RequestPointer->resource = resource;
        RequestPointer->resourceState = ResourceRequest::ResourceState::YES;
    }

    ResourceRequest::ResourceState ResourceAsk::getState()
    {
        std::scoped_lock lock(RequestPointer->mutex);
        return RequestPointer->resourceState;
    }

    std::string ResourceAsk::getID()
    {
        return RequestPointer->resourceID;
    }

    std::vector<size_t> ResourceAsk::getArguments()
    {
        return RequestPointer->resourceArgs;
    }

    void ResourceAsk::invalidate()
    {
        std::scoped_lock lock(RequestPointer->mutex);
        RequestPointer->resource = nullptr;
        RequestPointer->resourceState = ResourceRequest::ResourceState::DEAD;
    }
}