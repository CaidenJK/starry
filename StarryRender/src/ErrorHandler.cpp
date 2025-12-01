#include "Asset.h"

#include <iostream>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

namespace StarryRender 
{
	std::shared_ptr<ErrorHandler> ErrorHandler::globalErrorHandler = nullptr;

	std::weak_ptr<ErrorHandler> ErrorHandler::get() 
	{
		if (globalErrorHandler == nullptr) {
			globalErrorHandler.reset(new ErrorHandler());
		}
		return std::weak_ptr<ErrorHandler>(globalErrorHandler);
	}

	void ErrorHandler::registerAsset(RenderAsset* asset) 
	{
		if (asset == nullptr) {
			return;
		}
		registeredAssets.insert({ asset->getUUID(), asset });
	}

	void ErrorHandler::unregisterAsset(uint64_t uuid) 
	{
		for (const auto& asset : registeredAssets) {
			if (asset.first == uuid) {
				registeredAssets.erase(asset.first);
				break;
			}
		}
	}

	void ErrorHandler::enumerateAssets() 
	{
		for (const auto& asset : registeredAssets) {
			if (asset.second->getAlert() && (asset.second->getAlertSeverity() != RenderAsset::CallSeverity::NONE)) {
				AssetCall call;
				call.callerUUID = asset.first;
				call.callerName = asset.second->getAssetName();
				call.message = asset.second->getAlertMessage();
				call.severity = asset.second->getAlertSeverity();
				alertMessageBuffer.push_back(call);
				if (call.severity == RenderAsset::CallSeverity::FATAL) { hasFatal = true; }
				if (call.severity == RenderAsset::CallSeverity::INFO_URGANT || 
					call.severity == RenderAsset::CallSeverity::CRITICAL ||
					call.severity == RenderAsset::CallSeverity::FATAL) {
					shouldFlush = true;
				}
			}
		}
		if (shouldFlush || (alertMessageBuffer.size() >= BUFFER_FLUSH_LIMIT)) {
			flushCalls();
		}
	}
	void ErrorHandler::flushCalls() 
	{
		if (alertMessageBuffer.size() == 0) {
			return;
		}
#ifdef SUCCESS_VALIDATION
		std::cerr << "\n----------> Error Handler caught the following alerts:\n" << std::endl;
		for (const auto& call : alertMessageBuffer) {
			std::cerr << "[" << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << call.callerUUID << "\": \n\t" << call.message << "\n";
		}
		std::cerr << std::endl;
#endif
		alertMessageBuffer.clear();
		shouldFlush = false;
	}

	std::string ErrorHandler::severityToString(RenderAsset::CallSeverity severity) 
	{
		switch (severity) {
		case RenderAsset::CallSeverity::NONE:
			return "NONE";
		case RenderAsset::CallSeverity::INFO:
			return "INFO";
		case RenderAsset::CallSeverity::INFO_URGANT:
			return "INFO";
		case RenderAsset::CallSeverity::WARNING:
			return "WARNING";
		case RenderAsset::CallSeverity::CRITICAL:
			return "CRITICAL";
		case RenderAsset::CallSeverity::FATAL:
			return "!!!FATAL!!!";
		default:
			return "NONE";
		}
	}
}