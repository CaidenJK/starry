#pragma once

#include <vector>
#include <string>
#include <memory>
#include <random>
#include <ctime>

namespace StarryRender {
	class RenderAsset;

	class ErrorHandler {
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
			std::string& getLastErrorMessage() { return lastErrorMessage; }

		private:
			ErrorHandler() {}

			static std::shared_ptr<ErrorHandler> globalErrorHandler;
			std::vector<RenderAsset*> registeredAssets;

			bool isError = false;
			std::string lastErrorMessage = "";
	};

	class RenderAsset {
		public:
			RenderAsset();
			~RenderAsset();
			
			bool getError(std::string& outMessage);
			bool getError() { return error; }

			uint64_t getUUID() { return uuid; }
		protected:
			void registerError(const std::string& message);
		private:
			static uint64_t generateUUID();
			static std::mt19937_64 randomGen;

			bool error = false;
			std::string errorMessage = "";

			uint64_t uuid = 0;
	};
}