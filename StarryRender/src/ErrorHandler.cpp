#include "Asset.h"

#include <iostream>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

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
		registeredAssets.insert({ asset->getUUID(), asset});
	}

	void ErrorHandler::unregisterAsset(uint64_t uuid) {
		for (const auto& asset : registeredAssets) {
			if (asset.first == uuid) {
				registeredAssets.erase(asset.first);
				break;
			}
		}
	}

	void ErrorHandler::enumerateErrors() {
		for (const auto& asset : registeredAssets) {
			if (asset.second->getError()) {
				isError = true;
				AssetCall call;
				call.callerUUID = asset.first;
				call.callerName = asset.second->getAssetName();
				call.message = asset.second->getErrorMessage();
				errorMessageBuffer.push_back(call);
			}
			if (asset.second->getWarning()) {
				AssetCall call;
				call.callerUUID = asset.first;
				call.callerName = asset.second->getAssetName();
				call.message = asset.second->getWarningMessage();
				alertMessageBuffer.push_back(call);
			}
		}
		flushWarnings();
		flushErrors();
	}
	void ErrorHandler::flushErrors() {
		if (errorMessageBuffer.size() == 0) {
			return;
		}
#ifdef SUCCESS_VALIDATION
		std::cerr << "\n----------> Error Handler caught the following errors:\n" << std::endl;
		for (const auto& message : errorMessageBuffer) {
			std::cerr << "[FATAL] - " << "Clr: " << message.callerName << ", \"" << message.callerUUID << "\": \n\t" << message.message;
		}
		std::cerr << std::endl;
#endif
		errorMessageBuffer.clear();
	}

	void ErrorHandler::flushWarnings() {
		if (alertMessageBuffer.size() == 0) {
			return;
		}
#ifdef SUCCESS_VALIDATION
		std::cout << "\n----------> Caught alerts:\n" << std::endl;
		for (const auto& message : alertMessageBuffer) {
			std::cout << "- " << "Clr: " << message.callerName << ", \"" << message.callerUUID << "\": \n\t" << message.message;
		}
		std::cout << std::endl;
#endif
		alertMessageBuffer.clear();
	}
}