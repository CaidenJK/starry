#include "AssetManager.h"

namespace StarryLog
{
    ResourceRequest::ResourceRequest(uint64_t callerUUID, uint64_t senderUUID, size_t resourceID) :
    callerUUID(callerUUID), senderUUID(senderUUID), resourceID(resourceID)
    {

    }

    std::shared_ptr<ResourceRequest> ResourceRequest::create(uint64_t caller, uint64_t sender, size_t id) 
    {
        return std::shared_ptr<ResourceRequest>(new ResourceRequest(caller, sender, id));
    }
}