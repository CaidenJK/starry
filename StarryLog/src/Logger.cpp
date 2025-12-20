#include "Asset.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>

namespace StarryLog
{
	std::shared_ptr<Logger> Logger::globalLogger = nullptr;
	bool Logger::isDead = false;

	Logger::Logger() : StarryAsset(false) 
	{
		alertQueue.store(new std::queue<AssetCall>());
		loggingThread = std::thread(&Logger::worker, this);
		registerAsset(this);
	}

	Logger::~Logger()
	{
		hasFatal.store(true);
		loggingThread.join();
		delete alertQueue.load();
		alertQueue.store(nullptr);
		isDead = true;
	}

	void Logger::registerAlert(const std::string& message, CallSeverity severity)
	{
		registryMutex.lock();

		AssetCall call;
		call.callerUUID = getUUID();
		call.callerName = getAssetName();
		call.message = message;
		call.severity = severity;
		call.callTime = std::chrono::system_clock::now();
		alertQueue.load()->push(call);
		
		registryMutex.unlock();
	}

	bool Logger::isFatal()
	{ 
		return hasFatal.load(); 
	}

	std::weak_ptr<Logger> Logger::get() 
	{
		if (isDead) return {};
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
		registryMutex.lock();
		registeredAssets.insert({ asset->getUUID(), asset });
		registryMutex.unlock();
	}

	void Logger::unregisterAsset(uint64_t uuid) 
	{
		if (alertQueue.load() == nullptr) return;
		registryMutex.lock();
		while (!alertQueue.load()->empty()) {}
		registeredAssets.erase(uuid);
		registryMutex.unlock();
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

	void Logger::worker() {
		while (!hasFatal.load()) {
			if (!alertQueue.load()->empty()) {
				logAlert(alertQueue.load()->front());
				alertQueue.load()->pop();
			}
		}
	}

	void Logger::registerAssetAlert(uint64_t uuid)
	{
		registryMutex.lock();
		auto asset = registeredAssets.find(uuid);
		if (asset == registeredAssets.end() || asset->second == nullptr) {
			registryMutex.unlock();
			return;
		}
		AssetCall call;
		call.callerUUID = asset->first;
		call.callerName = asset->second->getAssetName();
		call.message = asset->second->getAlertMessage();
		call.severity = asset->second->getAlertSeverity();
		call.callTime = std::chrono::system_clock::now();
		alertQueue.load()->push(call);
		asset->second->resetAlert();
		registryMutex.unlock();
	}

	void Logger::logAlert(AssetCall& call) {
#ifdef NDEBUG
		if (call.severity == StarryAsset::CallSeverity::INFO || call.severity == StarryAsset::CallSeverity::INFO_URGANT) {
			return;
		}
#endif
		toFlushBuffer.push_back(call);
		callHistory.push_back(call);

		if ((call.severity != StarryAsset::CallSeverity::NONE)) {			
			if (call.severity == StarryAsset::CallSeverity::FATAL) hasFatal.store(true);
			if (call.severity == StarryAsset::CallSeverity::INFO_URGANT ||
				call.severity == StarryAsset::CallSeverity::BANNER ||
				call.severity == StarryAsset::CallSeverity::CRITICAL ||
				call.severity == StarryAsset::CallSeverity::FATAL) {
				shouldFlush.store(true);
			}
		}
		if (shouldFlush.load() || (toFlushBuffer.size() >= BUFFER_FLUSH_LIMIT)) {
			flushCalls();
			std::cout.flush();
		}
		if (hasFatal.load() && hasExitRights.load()) {
			registerAlert("A fatal alert was registered. Exiting program as permitted by Logger rights.", INFO);
			Logger::~Logger();
			std::exit(EXIT_FAILURE);
		}
	}

	void Logger::flushCalls() 
	{
		if (toFlushBuffer.size() == 0) {
			return;
		}
		std::cerr << "\n---> Caught alerts:\n" << std::endl;
		for (const auto& call : toFlushBuffer) {
			if (logToFile.load()) {
				dumpToFile(call);
			}
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

		toFlushBuffer.clear();
		shouldFlush.store(false);
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

	void Logger::dumpToFile(const Logger::AssetCall& call)
	{
		if (!didLogToFile) {
			std::filesystem::create_directories(LOG_PATH);
		}
		std::fstream f;
		f.open(LOG_FILE, std::ios::app | std::fstream::out);
		
		if (!f) {
			registerAlert("Couldn't open Log File!", FATAL);
			return;
		}

		if (!didLogToFile) {
			f << LOG_HEADER << "\n" << std::endl;
			didLogToFile = true;
		}
		std::time_t tt = std::chrono::system_clock::to_time_t(call.callTime);
		std::tm* lt = std::localtime(&tt);
		f << "[" << lt->tm_year + 1900 << "-" << lt->tm_mon + 1 << "-" << lt->tm_mday << " " << lt->tm_hour << ":" << lt->tm_min << ":" << lt->tm_sec
			<< " | " << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << call.callerUUID << "\" => \n\t" << call.message << "\t<=\n" << std::endl;
		f.close();
	}

	void Logger::dumpRegisteredAssets(bool names)
	{
		registryMutex.lock();
		size_t registeredAssetsSize = registeredAssets.size();
		std::vector<std::string> pointerArray;
		std::vector<std::string> idArray;
		std::vector<std::string> nameArray;
		for (const auto& asset : registeredAssets) {
			pointerArray.push_back(std::to_string((size_t)(asset.second)));
			idArray.push_back(std::to_string(asset.first));
			nameArray.push_back(std::string(asset.second->getAssetName()));
		}
		registryMutex.unlock();
		
		registerAlert("Logger: Asset Dump - Total = " + std::to_string(registeredAssetsSize) + "\n", INFO);
		for (int i = 0; i < pointerArray.size(); i++) {
			registerAlert("  Id = " + idArray[i] + ", Address = " + pointerArray[i] + ", Name = " + nameArray[i] + "\n", INFO);
		}
		flushCalls();
	}
}