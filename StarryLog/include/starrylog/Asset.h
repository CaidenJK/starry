#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>
#include <cstdint>
#include <chrono>

#define LOG_PATH "starry_out/log"
#define LOG_HEADER \
	"-------------------------" \
	"  --- Start Session ---  " \
	"-------------------------"


namespace StarryLog
{
	class StarryAsset {
	public:
		~StarryAsset();

		StarryAsset(const StarryAsset& other) = delete;
		StarryAsset& operator=(const StarryAsset& other) = delete;
		// TODO: StarryAsset& StarryAsset::Copy(const StarryAsset&); Custom copy method

		StarryAsset(StarryAsset&& other) noexcept;
		StarryAsset& operator=(StarryAsset&& other) noexcept;

		enum CallSeverity {
			NONE,
			INFO,
			INFO_URGANT,
			BANNER,
			WARNING,
			CRITICAL,
			FATAL
		};
		// TODO: Silent and File Logging

		bool getAlert() { return hasAlert; }
		std::string& getAlertMessage() { return alertMessage; }
		CallSeverity getAlertSeverity() { return assetState; }

		void resetAlert();

		virtual const std::string getAssetName() = 0;

		uint64_t getUUID() { return uuid; }

		std::mutex alertMutex;

	protected:
		StarryAsset();
		void registerAlert(const std::string& message, CallSeverity severity);

	private:
		static uint64_t generateUUID();
		static std::mt19937_64 randomGen;

		bool hasAlert = false;
		CallSeverity assetState = NONE;
		std::string alertMessage = "";

		uint64_t uuid = 0;
	};

	class Logger {
		struct AssetCall {
			uint64_t callerUUID;
			std::string callerName;
			std::string message;
			StarryAsset::CallSeverity severity;
			std::chrono::time_point<std::chrono::system_clock> callTime = std::chrono::system_clock::now();
		};

	public:
		~Logger();

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		// Note references are always valid since they are unregistered on asset destruction
		void registerAsset(StarryAsset* asset);
		void unregisterAsset(uint64_t uuid);
		void updateAssetPointer(uint64_t uuid, StarryAsset* newPtr);

		static std::weak_ptr<Logger> get();

		void registerAlert(uint64_t uuid);
		
		void dumpRegisteredAssets(bool names);
		void setExitRights(bool rights) { hasExitRights.store(rights); }
		void setFileLogging(bool value) { logToFile.store(value); }

		bool isFatal();

	private:
		Logger();

		void flushCalls();
		void logAlert(uint64_t uuid);
		void dumpToFile(const AssetCall& call);

		void worker();

		static std::string severityToString(StarryAsset::CallSeverity severity);

		const char* LOG_FILE = LOG_PATH "/log_out.txt";
		bool didLogToFile = false;
		static const int BUFFER_FLUSH_LIMIT = 5;

		static std::shared_ptr<Logger> globalLogger;
		std::map<uint64_t, StarryAsset*> registeredAssets;
		std::mutex registryMutex;

		bool shouldFlush = false;
		std::atomic<bool> hasFatal = false;

		std::atomic<uint64_t> alertUUID = 0;
		std::atomic<bool> logToFile = false;
		std::atomic<bool> hasExitRights = false;
		std::vector<AssetCall> toFlushBuffer = {};

		std::vector<AssetCall> callHistory = {};

		std::thread loggingThread;
	};
}