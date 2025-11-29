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

	bool RenderAsset::getError(std::string& outMessage) {
		if (error) {
			outMessage = errorMessage;
		}
		return error;
	}

	void RenderAsset::registerError(const std::string& message) {
		error = true;
		errorMessage = message;
		ErrorHandler::get().lock()->enumerateErrors();
	}

	uint64_t RenderAsset::generateUUID() {
		return randomGen();
	}
}