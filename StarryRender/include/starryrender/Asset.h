#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <random>

namespace StarryRender {
	class RenderAsset;

	class ErrorHandler {
		struct AssetCall {
			uint64_t callerUUID;
			std::string callerName;
			std::string message;
		};

		public:
			ErrorHandler(const ErrorHandler&) = delete;
			ErrorHandler& operator=(const ErrorHandler&) = delete;

			// Note references are always valid since they are unregistered on asset destruction
			void registerAsset(RenderAsset* asset);
			void unregisterAsset(uint64_t uuid);

			static std::weak_ptr<ErrorHandler> get();

			// Bool for now
			void enumerateErrors();

			bool hasError() { return isError; }

		private:
			ErrorHandler() {}

			void flushErrors();
			void flushWarnings();

			static std::shared_ptr<ErrorHandler> globalErrorHandler;
			std::map<uint64_t, RenderAsset*> registeredAssets;

			bool isError = false;
			std::vector<AssetCall> errorMessageBuffer = {};
			std::vector<AssetCall> alertMessageBuffer = {};
	};

	class RenderAsset {
		public:
			RenderAsset();
			~RenderAsset();
			
			bool getError() { return error; }
			std::string& getErrorMessage() { return errorMessage; }

			bool getWarning() { return hasAlert; }
			std::string& getWarningMessage() { return alertMessage; }

			virtual const std::string getAssetName() = 0;

			uint64_t getUUID() { return uuid; }
		protected:
			void registerError(const std::string& message);
			void registerAlert(const std::string& message);

		private:
			static uint64_t generateUUID();
			static std::mt19937_64 randomGen;

			bool error = false;
			std::string errorMessage = "";

			bool hasAlert = false;
			std::string alertMessage = "";

			uint64_t uuid = 0;
	};
}