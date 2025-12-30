#include "Logger.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <format>

namespace StarryManager
{
	bool Logger::isDead = false;

	Logger::Logger() : StarryAsset(false)
	{
		alertQueue = new std::queue<AssetCall>();
		loggingThread = std::thread(&Logger::worker, this);
	}

	Logger::~Logger()
	{
		hasFatal.store(true);
		loggingThread.join();
		
		std::scoped_lock lock(queueMutex);
		delete alertQueue;
		alertQueue = nullptr;
		isDead = true;
	}

	void Logger::registerAlert(const std::string& message, CallSeverity severity)
	{
		AssetCall call;
		call.callerUUID = getUUID();
		call.callerName = getAssetName();
		call.message = message;
		call.severity = severity;
		call.callTime = std::chrono::system_clock::now();
		std::scoped_lock lock(queueMutex);
		alertQueue->push(call);
		queueCV.notify_all();
	}

	void Logger::enqueueAlert(AssetCall& call)
	{
		std::scoped_lock lock(queueMutex);
		alertQueue->push(call);
		queueCV.notify_all();
	}

	void Logger::checkQueue()
	{
		std::unique_lock lock(queueMutex);
		queueCV.wait(lock, [this]() { return !alertQueue->empty() && !hasFatal.load(); });
		logAlert(alertQueue->front());
		alertQueue->pop();

		if (shouldFlush.load()) flushCalls();
	}

	void Logger::worker() 
	{
		while (!hasFatal.load()) {
			checkQueue();
		}
		if (shouldFlush.load()) {
			flushCalls();
		}
	}

	void Logger::logAlert(AssetCall& call) {
#ifdef NDEBUG
		if (call.severity == INFO || call.severity == INFO_URGANT) {
			return;
		}
#endif
		toFlushBuffer.push_back(call);
		callHistory.push_back(call);

		if ((call.severity != NONE)) {			
			if (call.severity == FATAL) hasFatal.store(true);
			if (call.severity == INFO_URGANT ||
				call.severity == BANNER ||
				call.severity == CRITICAL ||
				call.severity == FATAL) {
				shouldFlush.store(true);
			}
		}
		if (shouldFlush.load() || (toFlushBuffer.size() >= BUFFER_FLUSH_LIMIT)) {
			flushCalls();
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
				<< " | " << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << std::format("{:#016x}", call.callerUUID) << "\" => \n\t" << call.message << "\n";
		}
		std::cerr << std::endl;
		std::cout.flush();

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
			<< " | " << severityToString(call.severity) << "] - " << "Clr: " << call.callerName << ", \"" << std::format("{:#016x}", call.callerUUID) << "\" => \n\t" << call.message << "\t<=\n" << std::endl;
		f.close();
	}

	void Logger::flushQueueBlock()
	{
		std::scoped_lock lock(queueMutex);
		if (alertQueue == nullptr) {
			return;
		}
		while (!alertQueue->empty()) {}
	}
}