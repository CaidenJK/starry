#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <random>
#include <cstdint>

#define MAX_FRAMES_IN_FLIGHT 2

namespace StarryRender 
{
	class StarryLog::StarryAsset {
	public:
		~StarryLog::StarryAsset();

		enum CallSeverity {
			NONE,
			INFO,
			INFO_URGANT,
			WARNING,
			CRITICAL,
			FATAL
		};

		bool getAlert() { return hasAlert; }
		std::string& getAlertMessage() { return alertMessage; }
		CallSeverity getAlertSeverity() { return assetState; }

		virtual const std::string getAssetName() = 0;

		uint64_t getUUID() { return uuid; }

	protected:
		StarryLog::StarryAsset();
		void registerAlert(const std::string& message, CallSeverity severity);

	private:
		static uint64_t generateUUID();
		static std::mt19937_64 randomGen;

		bool hasAlert = false;
		CallSeverity assetState = NONE;
		std::string alertMessage = "";

		uint64_t uuid = 0;
	};

	class StarryLog::Logger {
		struct AssetCall {
			uint64_t callerUUID;
			std::string callerName;
			std::string message;
			StarryLog::StarryAsset::CallSeverity severity;
		};

	public:
		StarryLog::Logger(const StarryLog::Logger&) = delete;
		StarryLog::Logger& operator=(const StarryLog::Logger&) = delete;

		// Note references are always valid since they are unregistered on asset destruction
		void registerAsset(StarryLog::StarryAsset* asset);
		void unregisterAsset(uint64_t uuid);

		static std::weak_ptr<StarryLog::Logger> get();

		// Can call publicly
		bool enumerateAssets();

		bool isFatal() { return hasFatal; }

	private:
		StarryLog::Logger() {}

		void flushCalls();

		static std::string severityToString(StarryLog::StarryAsset::CallSeverity severity);

		const int BUFFER_FLUSH_LIMIT = 5;

		static std::shared_ptr<StarryLog::Logger> globalErrorHandler;
		std::map<uint64_t, StarryLog::StarryAsset*> registeredAssets;

		bool shouldFlush = false;
		bool hasFatal = false;
		std::vector<AssetCall> alertMessageBuffer = {};
	};
	// TODO: Call print immediately (timer) 
}