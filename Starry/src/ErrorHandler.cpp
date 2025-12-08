#include "Asset.h"

#include <iostream>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

namespace Starry
{
	std::shared_ptr<ErrorHandler> ErrorHandler::globalErrorHandler = nullptr;

	std::weak_ptr<ErrorHandler> ErrorHandler::get() 
	{
		if (globalErrorHandler == nullptr) {
			globalErrorHandler.reset(new ErrorHandler());
		}
		return std::weak_ptr<ErrorHandler>(globalErrorHandler);
	}

	void ErrorHandler::registerAsset(StarryAsset* asset) 
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

	bool ErrorHandler::enumerateAssets() 
	{
		for (const auto& asset : registeredAssets) {
			if (asset.second->getAlert() && (asset.second->getAlertSeverity() != StarryAsset::CallSeverity::NONE)) {
				AssetCall call;
				call.callerUUID = asset.first;
				call.callerName = asset.second->getAssetName();
				call.message = asset.second->getAlertMessage();
				call.severity = asset.second->getAlertSeverity();
				alertMessageBuffer.push_back(call);
				if (call.severity == StarryAsset::CallSeverity::FATAL) { hasFatal = true; }
				if (call.severity == StarryAsset::CallSeverity::INFO_URGANT ||
					call.severity == StarryAsset::CallSeverity::CRITICAL ||
					call.severity == StarryAsset::CallSeverity::FATAL) {
					shouldFlush = true;
				}
			}
		}
		if (shouldFlush || (alertMessageBuffer.size() >= BUFFER_FLUSH_LIMIT)) {
			flushCalls();
		}
		return hasFatal;
	}

	void ErrorHandler::flushCalls() 
	{
		if (alertMessageBuffer.size() == 0) {
			return;
		}
#ifdef SUCCESS_VALIDATION
		std::cerr << "\n----------> Starry caught the following alerts:\n" << std::endl;
		for (const auto& call : alertMessageBuffer) {
			std::cerr << "[" << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << call.callerUUID << "\": \n\t" << call.message << "\n";
		}
		std::cerr << std::endl;
#endif
		alertMessageBuffer.clear();
		shouldFlush = false;
	}

	std::string ErrorHandler::severityToString(StarryAsset::CallSeverity severity)
	{
		switch (severity) {
		case StarryAsset::CallSeverity::NONE:
			return "NONE";
		case StarryAsset::CallSeverity::INFO:
			return "INFO";
		case StarryAsset::CallSeverity::INFO_URGANT:
			return "INFO";
		case StarryAsset::CallSeverity::WARNING:
			return "WARNING";
		case StarryAsset::CallSeverity::CRITICAL:
			return "CRITICAL";
		case StarryAsset::CallSeverity::FATAL:
			return "!!!FATAL!!!";
		default:
			return "NONE";
		}
	}
}