#include "Asset.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#ifndef NDEBUG
	#define SUCCESS_VALIDATION
#endif

namespace StarryLog
{
	std::shared_ptr<Logger> Logger::globalLogger = nullptr;

	Logger::~Logger() {
		if (loggingThread.joinable()) {
			loggingThread.join();
		}
	}

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
		registryMutex.lock();
		registeredAssets.insert({ asset->getUUID(), asset });
		registryMutex.unlock();
		//std::cerr << "[Logger@" << this << "] registerAsset id=" << asset->getUUID() << " ptr=" << asset << std::endl;
	}

	void Logger::unregisterAsset(uint64_t uuid) 
	{
		//std::cerr << "[Logger@" << this << "] unregisterAsset id=" << uuid << "\n";
		registryMutex.lock();
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

	void Logger::registerAlert(uint64_t uuid)
	{
		if (loggingThread.joinable()) {
			loggingThread.join();
		}

		loggingThread = std::thread(&Logger::logAlert, this, uuid);
		loggingThread.detach();
	}

	void Logger::logAlert(uint64_t uuid) {
		registryMutex.lock();
		auto asset = registeredAssets.find(uuid);
		asset->second->alertMutex.lock();
		if (asset != registeredAssets.end() && asset->second->getAlert() &&
			(asset->second->getAlertSeverity() != StarryAsset::CallSeverity::NONE)) {
			AssetCall call;
			call.callerUUID = asset->first;
			call.callerName = asset->second->getAssetName();
			call.message = asset->second->getAlertMessage();
			call.severity = asset->second->getAlertSeverity();
			call.callTime = std::chrono::system_clock::now();
			toFlushBuffer.push_back(call);
			callHistory.push_back(call);
			asset->second->alertMutex.unlock();

			asset->second->resetAlert();

			if (call.severity == StarryAsset::CallSeverity::FATAL) hasFatal.store(true);
			if (call.severity == StarryAsset::CallSeverity::INFO_URGANT ||
				call.severity == StarryAsset::CallSeverity::BANNER ||
				call.severity == StarryAsset::CallSeverity::CRITICAL ||
				call.severity == StarryAsset::CallSeverity::FATAL) {
				shouldFlush = true;
			}
		}
		if (shouldFlush || (toFlushBuffer.size() >= BUFFER_FLUSH_LIMIT)) {
			flushCalls();
			std::cout.flush();
		}
		if (hasFatal.load() && hasExitRights.load()) {
			std::cerr << "A fatal alert was registered. Exiting program as permitted by Logger rights." << std::endl;
			std::exit(EXIT_FAILURE);
		}
		registryMutex.unlock();
	}

	void Logger::flushCalls() 
	{
		if (toFlushBuffer.size() == 0) {
			return;
		}
#ifdef SUCCESS_VALIDATION
		std::cerr << "\n---> Caught alerts:\n" << std::endl;
#endif
		for (const auto& call : toFlushBuffer) {
			if (logToFile.load()) {
				dumpToFile(call);
			}
#ifdef SUCCESS_VALIDATION
			if (call.severity == StarryAsset::CallSeverity::BANNER) {
				std::cout << call.message << "\n";
				continue;
			}
			std::time_t tt = std::chrono::system_clock::to_time_t(call.callTime);
			std::tm* lt = std::localtime(&tt);
			std::cerr << "[" << lt->tm_year+1900 << "-" << lt->tm_mon+1 << "-" << lt->tm_mday << " " << lt->tm_hour << ":" << lt->tm_min << ":" << lt->tm_sec
				<< " | " << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << call.callerUUID << "\" => \n\t" << call.message << "\n";
#endif
		}
#ifdef SUCCESS_VALIDATION
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

	void Logger::dumpToFile(const Logger::AssetCall& call)
	{
		if (!didLogToFile) {
			std::filesystem::create_directories(LOG_PATH);
		}
		std::fstream f;
		f.open(LOG_FILE, std::ios::app | std::fstream::out);
		
		if (!f) {
			std::cerr << "Couldn't open Log File!" << std::endl;
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
		registryMutex.unlock();
	}
}