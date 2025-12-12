#include "Asset.h"

namespace StarryLog
{
	std::mt19937_64 StarryAsset::randomGen = std::mt19937_64(std::time(nullptr));

	StarryAsset::StarryAsset()
	{
		uuid = generateUUID();
		
		Logger::get().lock()->registerAsset(this);
	}

	StarryAsset::~StarryAsset()
	{
		Logger::get().lock()->unregisterAsset(uuid);
	}

	StarryAsset::StarryAsset(StarryAsset&& other) noexcept
		: hasAlert(other.hasAlert),
		assetState(other.assetState),
		alertMessage(std::move(other.alertMessage)),
		uuid(other.uuid)
	{
		other.uuid = 0; // saftey
		if (auto lg = Logger::get().lock()) {
			lg->updateAssetPointer(uuid, this);
		}
	}

	StarryAsset& StarryAsset::operator=(StarryAsset&& other) noexcept
	{
		if (this != &other) {
			if (uuid != 0) {
				Logger::get().lock()->unregisterAsset(uuid);
			}
			hasAlert = other.hasAlert;
			assetState = other.assetState;
			alertMessage = std::move(other.alertMessage);
			uuid = other.uuid;

			other.uuid = 0; // saftey
			if (auto lg = Logger::get().lock()) {
				lg->updateAssetPointer(uuid, this);
			}
		}
		return *this;
	}

	void StarryAsset::registerAlert(const std::string& message, CallSeverity severity)
	{
		alertMutex.lock();
		hasAlert = true;
		alertMessage = message;
		assetState = severity;
		alertMutex.unlock();

		Logger::get().lock()->registerAlert(uuid);
	}

	void StarryAsset::resetAlert() {
		alertMutex.lock();

		if (assetState != FATAL) {
			hasAlert = false;
			alertMessage.clear();
			assetState = NONE;
		}
		alertMutex.unlock();
	}

	uint64_t StarryAsset::generateUUID()
	{
		return randomGen();
	}
}