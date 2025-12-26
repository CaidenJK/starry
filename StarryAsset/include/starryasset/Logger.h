#pragma once

#include "StarryAsset.h"

#include <queue>

#define STARRY_OUT "starry_out"
#define LOG_PATH STARRY_OUT "/log"
#define LOG_HEADER \
	"-------------------------" \
	"  --- Start Session ---  " \
	"-------------------------"


namespace StarryAssets
{
    class Logger : public StarryAsset {
    public:
		struct AssetCall {
			uint64_t callerUUID;
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

		const std::string getAssetName() override {return "Logger";}

	private:
		void logAlert(AssetCall& call);
		void dumpToFile(const AssetCall& call);
		void flushCalls();

		void registerAlert(const std::string& message, CallSeverity severity) override;

		void worker();

		static std::string severityToString(StarryAsset::CallSeverity severity);

		const char* LOG_FILE = LOG_PATH "/log_out.txt";
		bool didLogToFile = false;
		static const int BUFFER_FLUSH_LIMIT = 5;

        //std::mutex alertMutex;

		std::atomic<bool> shouldFlush = false; // Bit flags soon
		std::atomic<bool> hasFatal = false;
		
        std::atomic<std::queue<AssetCall>*> alertQueue;

		std::atomic<bool> logToFile = false;
		std::atomic<bool> hasExitRights = false;
		
		std::vector<AssetCall> toFlushBuffer = {};
		std::vector<AssetCall> callHistory = {};

		std::thread loggingThread;

		static bool isDead;
	};
}