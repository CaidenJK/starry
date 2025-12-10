#include "Asset.h"

namespace StarryRender
{
	std::mt19937_64 StarryLog::StarryAsset::randomGen = std::mt19937_64(std::time(nullptr));

	StarryLog::StarryAsset::StarryLog::StarryAsset()
	{
		uuid = generateUUID();
		
		StarryLog::Logger::get().lock()->registerAsset(this);
	}

	StarryLog::StarryAsset::~StarryLog::StarryAsset()
	{
		StarryLog::Logger::get().lock()->unregisterAsset(uuid);
	}

	void StarryLog::StarryAsset::registerAlert(const std::string& message, CallSeverity severity)
	{
		hasAlert = true;
		alertMessage = message;
		assetState = severity;
		StarryLog::Logger::get().lock()->enumerateAssets();
		
		if (severity == CallSeverity::FATAL) {
			return;
		}
		hasAlert = false;
		alertMessage = "";
		assetState = CallSeverity::NONE;
	}

	uint64_t StarryLog::StarryAsset::generateUUID()
	{
		return randomGen();
	}
}