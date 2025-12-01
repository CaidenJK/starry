#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <random>
#include <cstdint>

namespace StarryRender 
{
	class RenderAsset {
	public:
		RenderAsset();
		~RenderAsset();

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
		void registerAlert(const std::string& message, CallSeverity severity);

	private:
		static uint64_t generateUUID();
		static std::mt19937_64 randomGen;

		bool hasAlert = false;
		CallSeverity assetState = NONE;
		std::string alertMessage = "";

		uint64_t uuid = 0;
	};

	class ErrorHandler {
		struct AssetCall {
			uint64_t callerUUID;
			std::string callerName;
			std::string message;
			RenderAsset::CallSeverity severity;
		};

	public:
		ErrorHandler(const ErrorHandler&) = delete;
		ErrorHandler& operator=(const ErrorHandler&) = delete;

		// Note references are always valid since they are unregistered on asset destruction
		void registerAsset(RenderAsset* asset);
		void unregisterAsset(uint64_t uuid);

		static std::weak_ptr<ErrorHandler> get();

		// Can call publicly
		void enumerateAssets();

		bool isFatal() { return hasFatal; }

	private:
		ErrorHandler() {}

		void flushCalls();

		static std::string severityToString(RenderAsset::CallSeverity severity);

		const int BUFFER_FLUSH_LIMIT = 5;

		static std::shared_ptr<ErrorHandler> globalErrorHandler;
		std::map<uint64_t, RenderAsset*> registeredAssets;

		bool shouldFlush = false;
		bool hasFatal = false;
		std::vector<AssetCall> alertMessageBuffer = {};
	};
	// TODO: Call print immediately (timer) 
}