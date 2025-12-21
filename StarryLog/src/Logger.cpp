#include "Logger.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>

namespace StarryLog
{
	bool Logger::isDead = false;

	Logger::Logger() : StarryAsset(false)
	{
		alertQueue.store(new std::queue<AssetCall>());
		loggingThread = std::thread(&Logger::worker, this);
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
		AssetCall call;
		call.callerUUID = getUUID();
		call.callerName = getAssetName();
		call.message = message;
		call.severity = severity;
		call.callTime = std::chrono::system_clock::now();
		alertQueue.load()->push(call);
	}

	void Logger::enqueueAlert(AssetCall& call)
	{
		alertQueue.load()->push(call);
	}

	void Logger::worker() 
	{
		while (!hasFatal.load()) {
			if (!alertQueue.load()->empty()) {
				logAlert(alertQueue.load()->front());
				alertQueue.load()->pop();
			}
			else if (shouldFlush.load()) flushCalls();
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

	void Logger::flushQueueBlock()
	{
		while (!alertQueue.load()->empty()) {}
	}
}