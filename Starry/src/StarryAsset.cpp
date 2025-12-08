#include "Asset.h"

namespace Starry
{
	std::mt19937_64 StarryAsset::randomGen = std::mt19937_64(std::time(nullptr));

	StarryAsset::StarryAsset()
	{
		uuid = generateUUID();
		
		ErrorHandler::get().lock()->registerAsset(this);
	}

	StarryAsset::~StarryAsset()
	{
		ErrorHandler::get().lock()->unregisterAsset(uuid);
	}

	void StarryAsset::registerAlert(const std::string& message, CallSeverity severity)
	{
		hasAlert = true;
		alertMessage = message;
		assetState = severity;
		ErrorHandler::get().lock()->enumerateAssets();
		
		if (severity == CallSeverity::FATAL) {
			return;
		}
		hasAlert = false;
		alertMessage = "";
		assetState = CallSeverity::NONE;
	}

	uint64_t StarryAsset::generateUUID()
	{
		return randomGen();
	}
}