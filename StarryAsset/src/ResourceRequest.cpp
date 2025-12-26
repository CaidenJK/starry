#include "AssetManager.h"

namespace StarryAssets
{
    ResourceRequest::ResourceRequest(uint64_t callerUUID, uint64_t senderUUID, size_t resourceID, std::type_index& typeInfo) :
    callerUUID(callerUUID), senderUUID(senderUUID), resourceID(resourceID), typeInfo(typeInfo)
    {
    }

    std::shared_ptr<ResourceRequest> ResourceRequest::create(uint64_t caller, uint64_t sender, size_t id, std::type_index typeInfo)
    {
        return std::shared_ptr<ResourceRequest>(new ResourceRequest(caller, sender, id, typeInfo));
    }
}