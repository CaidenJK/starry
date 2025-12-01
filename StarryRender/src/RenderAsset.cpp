#include "Asset.h"

namespace StarryRender 
{
	std::mt19937_64 RenderAsset::randomGen = std::mt19937_64(std::time(nullptr));

	RenderAsset::RenderAsset() 
	{
		uuid = generateUUID();
		
		ErrorHandler::get().lock()->registerAsset(this);
	}

	RenderAsset::~RenderAsset() 
	{
		ErrorHandler::get().lock()->unregisterAsset(uuid);
	}

	void RenderAsset::registerAlert(const std::string& message, CallSeverity severity) 
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

	uint64_t RenderAsset::generateUUID() 
	{
		return randomGen();
	}
}