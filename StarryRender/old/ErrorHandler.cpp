#include "Asset.h"

#include <iostream>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

namespace StarryRender
{
	std::shared_ptr<StarryLog::Logger> StarryLog::Logger::globalErrorHandler = nullptr;

	std::weak_ptr<StarryLog::Logger> StarryLog::Logger::get() 
	{
		if (globalErrorHandler == nullptr) {
			globalErrorHandler.reset(new StarryLog::Logger());
		}
		return std::weak_ptr<StarryLog::Logger>(globalErrorHandler);
	}

	void StarryLog::Logger::registerAsset(StarryLog::StarryAsset* asset) 
	{
		if (asset == nullptr) {
			return;
		}
		registeredAssets.insert({ asset->getUUID(), asset });
	}

	void StarryLog::Logger::unregisterAsset(uint64_t uuid) 
	{
		for (const auto& asset : registeredAssets) {
			if (asset.first == uuid) {
				registeredAssets.erase(asset.first);
				break;
			}
		}
	}

	bool StarryLog::Logger::enumerateAssets() 
	{
		for (const auto& asset : registeredAssets) {
			if (asset.second->getAlert() && (asset.second->getAlertSeverity() != StarryLog::StarryAsset::CallSeverity::NONE)) {
				AssetCall call;
				call.callerUUID = asset.first;
				call.callerName = asset.second->getAssetName();
				call.message = asset.second->getAlertMessage();
				call.severity = asset.second->getAlertSeverity();
				alertMessageBuffer.push_back(call);
				if (call.severity == StarryLog::StarryAsset::CallSeverity::FATAL) { hasFatal = true; }
				if (call.severity == StarryLog::StarryAsset::CallSeverity::INFO_URGANT ||
					call.severity == StarryLog::StarryAsset::CallSeverity::CRITICAL ||
					call.severity == StarryLog::StarryAsset::CallSeverity::FATAL) {
					shouldFlush = true;
				}
			}
		}
		if (shouldFlush || (alertMessageBuffer.size() >= BUFFER_FLUSH_LIMIT)) {
			flushCalls();
		}
		return hasFatal;
	}

	void StarryLog::Logger::flushCalls() 
	{
		if (alertMessageBuffer.size() == 0) {
			return;
		}
#ifdef SUCCESS_VALIDATION
		std::cerr << "\n----------> Renderer caught the following alerts:\n" << std::endl;
		for (const auto& call : alertMessageBuffer) {
			std::cerr << "[" << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << call.callerUUID << "\": \n\t" << call.message << "\n";
		}
		std::cerr << std::endl;
#endif
		alertMessageBuffer.clear();
		shouldFlush = false;
	}

	std::string StarryLog::Logger::severityToString(StarryLog::StarryAsset::CallSeverity severity)
	{
		switch (severity) {
		case StarryLog::StarryAsset::CallSeverity::NONE:
			return "NONE";
		case StarryLog::StarryAsset::CallSeverity::INFO:
			return "INFO";
		case StarryLog::StarryAsset::CallSeverity::INFO_URGANT:
			return "INFO";
		case StarryLog::StarryAsset::CallSeverity::WARNING:
			return "WARNING";
		case StarryLog::StarryAsset::CallSeverity::CRITICAL:
			return "CRITICAL";
		case StarryLog::StarryAsset::CallSeverity::FATAL:
			return "!!!FATAL!!!";
		default:
			return "NONE";
		}
	}
}