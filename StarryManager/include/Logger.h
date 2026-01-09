#pragma once

#include "StarryAsset.h"

#include <queue>
#include <condition_variable>

#define STARRY_OUT "starry_out"
#define LOG_PATH STARRY_OUT "/log"
#define LOG_HEADER \
	"-------------------------" \
	"  --- Start Session ---  " \
	"-------------------------"


namespace StarryManager
{
    class Logger : public StarryAsset {
    public:
		struct AssetCall {
			size_t callerUUID;
			std::string callerName;
			std::string message;
			StarryAsset::CallSeverity severity;
			std::chrono::time_point<std::chrono::system_clock> callTime = std::chrono::system_clock::now();
		};

	public:
        Logger();
		~Logger();

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		void enqueueAlert(AssetCall& call);
		
		void setFileLogging(bool value) { logToFile.store(value); }

		void toFlushCalls() { shouldFlush.store(true); }
        void flushQueueBlock();

		bool isFatal() { return hasFatal.load(); }
		
		static bool isLoggerDead() { return isDead; }

		ASSET_NAME("Logger")

	private:
		void logAlert(AssetCall& call);
		void dumpToFile(const AssetCall& call);
		
		void checkFlush();
		void flushCalls();

		void Alert(const std::string& message, CallSeverity severity) override;

		static std::string severityToString(StarryAsset::CallSeverity severity);

		const char* LOG_FILE = LOG_PATH "/log_out.txt";
		bool didLogToFile = false;
		static const int BUFFER_FLUSH_LIMIT = 5;


		std::atomic<bool> shouldFlush = false; // Bit flags soon
		std::atomic<bool> hasFatal = false;
		
		std::mutex queueMutex;
        std::queue<AssetCall>* alertQueue;

		std::atomic<bool> logToFile = false;
		
		std::vector<AssetCall> toFlushBuffer = {};
		std::vector<AssetCall> callHistory = {};

		std::condition_variable queueCV;
		std::thread loggingThread;
		void checkQueue();
		void worker();

		static bool isDead;
	};
}