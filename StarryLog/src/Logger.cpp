#include "Asset.h"

#include <iostream>
#include <cstdlib>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

namespace StarryLog
{
	std::shared_ptr<Logger> Logger::globalLogger = nullptr;

	std::weak_ptr<Logger> Logger::get() 
	{
		if (globalLogger == nullptr) {
			globalLogger.reset(new Logger());
		}
		return std::weak_ptr<Logger>(globalLogger);
	}

	void Logger::registerAsset(StarryAsset* asset) 
	{
		if (asset == nullptr) {
			return;
		}
		registeredAssets.insert({ asset->getUUID(), asset });
		//std::cerr << "[Logger@" << this << "] registerAsset id=" << asset->getUUID() << " ptr=" << asset << std::endl;
	}

	void Logger::unregisterAsset(uint64_t uuid) 
	{
		//std::cerr << "[Logger@" << this << "] unregisterAsset id=" << uuid << "\n";
		registeredAssets.erase(uuid);
	}

	void Logger::updateAssetPointer(uint64_t uuid, StarryAsset* newPtr) {
		if (uuid == 0 || newPtr == nullptr) {
			hasFatal = true;
			return;
		}
		auto it = registeredAssets.find(uuid);
		if (it != registeredAssets.end()) {
			it->second = newPtr;
		}
		else {
			hasFatal = true;
			return;
		}
		// TODO: add logger (this) to logger assets

	}

	bool Logger::enumerateAssets() 
	{
		for (const auto& asset : registeredAssets) {
			if (asset.second->getAlert() && (asset.second->getAlertSeverity() != StarryAsset::CallSeverity::NONE)) {
				AssetCall call;
				call.callerUUID = asset.first;
				call.callerName = asset.second->getAssetName();
				call.message = asset.second->getAlertMessage();
				call.severity = asset.second->getAlertSeverity();
				call.callTime = std::chrono::system_clock::now();
				toFlushBuffer.push_back(call);
				callHistory.push_back(call);
				if (call.severity == StarryAsset::CallSeverity::FATAL) { hasFatal = true; }
				if (call.severity == StarryAsset::CallSeverity::INFO_URGANT ||
					call.severity == StarryAsset::CallSeverity::BANNER ||
					call.severity == StarryAsset::CallSeverity::CRITICAL ||
					call.severity == StarryAsset::CallSeverity::FATAL) {
					shouldFlush = true;
				}
			}
		}
		if (shouldFlush || (toFlushBuffer.size() >= BUFFER_FLUSH_LIMIT)) {
			flushCalls();
		}
		if (hasFatal && hasExitRights) {
			std::cerr << "A fatal alert was registered. Exiting program as permitted by Logger rights." << std::endl;
			std::exit(EXIT_FAILURE);
		}
		return hasFatal;
	}

	void Logger::flushCalls() 
	{
		if (toFlushBuffer.size() == 0) {
			return;
		}
#ifdef SUCCESS_VALIDATION
		std::cerr << "\n---> Caught alerts:\n" << std::endl;
		for (const auto& call : toFlushBuffer) {
			if (call.severity == StarryAsset::CallSeverity::BANNER) {
				std::cout << call.message << "\n";
				continue;
			}
			std::time_t tt = std::chrono::system_clock::to_time_t(call.callTime);
			std::tm* lt = std::localtime(&tt);
			std::cerr << "[" << lt->tm_year+1900 << "-" << lt->tm_mon+1 << "-" << lt->tm_mday << " " << lt->tm_hour << ":" << lt->tm_min << ":" << lt->tm_sec
				<< " | " << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << call.callerUUID << "\" => \n\t" << call.message << "\n";
		}
		std::cerr << std::endl;
#endif
		toFlushBuffer.clear();
		shouldFlush = false;
	}

	std::string Logger::severityToString(StarryAsset::CallSeverity severity)
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
			return "NULL";
		}
	}

	void Logger::dumpRegisteredAssets(bool names)
	{
#ifdef SUCCESS_VALIDATION
		std::cerr << "Logger: Asset Dump - Total = " << registeredAssets.size() << std::endl;
		for (const auto& asset : registeredAssets) {
			const auto id = asset.first;
			const auto ptr = asset.second;
			std::cerr << "  Id = " << id << ", Address = " << ptr;
			if (names) {
				std::cerr << ", Name = " << ptr->getAssetName();
			}
			std::cerr << std::endl;
		}
		std::cerr << std::endl;
#endif
	}
}