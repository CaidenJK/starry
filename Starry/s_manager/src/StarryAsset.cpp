#include "StarryAsset.h"
#include "AssetManager.h"

namespace Manager
{
	std::mt19937_64 StarryAsset::randomGen = std::mt19937_64(std::time(nullptr));

	StarryAsset::StarryAsset()
	{
		uuid = generateUUID();
		
		AssetManager::get().lock()->registerAsset(this);
	}

	StarryAsset::StarryAsset(bool autoRegister) {
		uuid = generateUUID();
	}

	StarryAsset::~StarryAsset()
	{
		for (auto& ask : asks) {
			ask->invalidate();
		}

		if (auto mgr = AssetManager::get().lock()) mgr->unregisterAsset(uuid);
	}

	void StarryAsset::resourceAsk(std::shared_ptr<ResourceRequest>& Request)
	{
		// remove STALE
		std::erase_if(asks, [](std::shared_ptr<ResourceAsk>& ask) { return ask->getState() == ResourceRequest::STALE; });

		asks.emplace_back(std::make_shared<ResourceAsk>(Request)); 
		if (Request->resourceID.compare("self") == 0) {
			asks.back()->setResource((void*)this);
		}
		else {
			askCallback(asks.back());
		}
	}

	StarryAsset::StarryAsset(StarryAsset&& other) noexcept
		: hasAlert(other.hasAlert),
		assetState(other.assetState),
		alertMessage(std::move(other.alertMessage)),
		uuid(other.uuid)
	{
		other.uuid = 0; // saftey
		if (auto mgr = AssetManager::get().lock()) mgr->updateAssetPointer(uuid, this);
	}

	StarryAsset& StarryAsset::operator=(StarryAsset&& other) noexcept
	{
		if (this != &other) {
			if (uuid != 0) {
				if (auto mgr = AssetManager::get().lock()) mgr->unregisterAsset(uuid);
			}
			hasAlert = other.hasAlert;
			assetState = other.assetState;
			alertMessage = std::move(other.alertMessage);
			uuid = other.uuid;

			other.uuid = 0; // saftey
			if (auto mgr = AssetManager::get().lock()) mgr->updateAssetPointer(uuid, this);
		}
		return *this;
	}

	void StarryAsset::Alert(const std::string& message, CallSeverity severity)
	{
		hasAlert = true;
		alertMessage = message;
		assetState = severity;

		if (auto mgr = AssetManager::get().lock()) mgr->registerAssetAlert(uuid);
	}

	void StarryAsset::resetAlert() {

		if (assetState != FATAL) {
			hasAlert = false;
			alertMessage.clear();
			assetState = NONE;
		}
	}

	size_t StarryAsset::generateUUID()
	{
		return randomGen();
	}
}