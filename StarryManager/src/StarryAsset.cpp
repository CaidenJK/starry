#include "StarryAsset.h"
#include "AssetManager.h"

namespace StarryManager
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
		if (auto mgr = AssetManager::get().lock()) mgr->unregisterAsset(uuid);
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

	void StarryAsset::registerAlert(const std::string& message, CallSeverity severity)
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

	uint64_t StarryAsset::generateUUID()
	{
		return randomGen();
	}
}