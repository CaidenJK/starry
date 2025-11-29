#include "Asset.h"

namespace StarryRender {
	std::shared_ptr<ErrorHandler> ErrorHandler::globalErrorHandler = nullptr;

	std::weak_ptr<ErrorHandler> ErrorHandler::get() {
		if (globalErrorHandler == nullptr) {
			globalErrorHandler.reset(new ErrorHandler());
		}
		return std::weak_ptr<ErrorHandler>(globalErrorHandler);
	}

	void ErrorHandler::registerAsset(RenderAsset* asset) {
		if (asset == nullptr) {
			return;
		}
		registeredAssets.push_back(asset);
	}

	void ErrorHandler::unregisterAsset(uint64_t uuid) {
		for (auto it = registeredAssets.begin(); it != registeredAssets.end(); it++) {
			if ((*it)->getUUID() == uuid) {
				registeredAssets.erase(it);
				break;
			}
			else {
				registeredAssets.erase(it);
				break;
			}
		}
	}

	void ErrorHandler::enumerateErrors() {
		for (const auto& asset : registeredAssets) {
			isError = isError || asset->getError(lastErrorMessage);
		}
	}
}