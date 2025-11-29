#include "Asset.h"

namespace StarryRender {
	std::mt19937_64 RenderAsset::randomGen = std::mt19937_64(std::time(nullptr));

	RenderAsset::RenderAsset() {
		uuid = generateUUID();
		
		ErrorHandler::get().lock()->registerAsset(this);
	}

	RenderAsset::~RenderAsset() {
		ErrorHandler::get().lock()->unregisterAsset(uuid);
	}

	void RenderAsset::registerError(const std::string& message) {
		error = true;
		errorMessage = message;
		ErrorHandler::get().lock()->enumerateErrors();
	}

	void RenderAsset::registerAlert(const std::string& message) {
		hasAlert = true;
		alertMessage = message;
		ErrorHandler::get().lock()->enumerateErrors();
		
		hasAlert = false;
		alertMessage = "";
	}

	uint64_t RenderAsset::generateUUID() {
		return randomGen();
	}
}