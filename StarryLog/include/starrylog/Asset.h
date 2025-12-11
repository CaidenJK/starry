#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
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

		virtual const std::string getAssetName() = 0;

		uint64_t getUUID() { return uuid; }

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
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		// Note references are always valid since they are unregistered on asset destruction
		void registerAsset(StarryAsset* asset);
		void unregisterAsset(uint64_t uuid);
		void updateAssetPointer(uint64_t uuid, StarryAsset* newPtr);

		static std::weak_ptr<Logger> get();

		// Can call publicly
		bool enumerateAssets();
		void dumpToFile(const AssetCall& call);

		void dumpRegisteredAssets(bool names);

		void setExitRights(bool rights) { hasExitRights = rights; }
		void setFileLogging(bool value) { logToFile = value; }

		bool isFatal() { return hasFatal; }

	private:
		Logger() {}

		void flushCalls();

		static std::string severityToString(StarryAsset::CallSeverity severity);

		const char* LOG_FILE = LOG_PATH "/log_out.txt";
		bool didLogToFile = false;
		static const int BUFFER_FLUSH_LIMIT = 5;

		static std::shared_ptr<Logger> globalLogger;
		std::map<uint64_t, StarryAsset*> registeredAssets;

		bool shouldFlush = false;
		bool hasFatal = false;

		bool logToFile = false;
		bool hasExitRights = false;
		std::vector<AssetCall> toFlushBuffer = {};

		std::vector<AssetCall> callHistory = {};
	};
}