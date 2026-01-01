#include "Resource.h"

namespace StarryManager
{
    ResourceRequest::ResourceRequest(uint64_t callerUUID, uint64_t senderUUID, size_t resourceID, std::vector<size_t>& resourceArgs) :
    callerUUID(callerUUID), senderUUID(senderUUID), resourceID(resourceID), resourceArgs(resourceArgs)
    {
    }

    std::shared_ptr<ResourceRequest> ResourceRequest::create(uint64_t caller, uint64_t sender, size_t id, std::vector<size_t>& args)
    {
        return std::shared_ptr<ResourceRequest>(new ResourceRequest(caller, sender, id, args));
    }
}