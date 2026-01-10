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

#include "Resource.h"

#define INVALID_RESOURCE 400

#define ASSET_NAME(x) const std::string getAssetName() override { return x; }

#define GET_RESOURCE std::optional<void*> getResource(const size_t resourceID, const std::vector<size_t> resourceArgs) override
#define GET_RESOURCE_FROM_STRING size_t getResourceIDFromString(const std::string resourceName) override

namespace StarryManager
{
	template <typename T>
	class ResourceHandle;

	class StarryAsset {
	public:
		virtual ~StarryAsset();

		StarryAsset(const StarryAsset& other) = default;
		StarryAsset& operator=(const StarryAsset& other) = default;
		// TODO: Custom copy method

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
		// TODO: Add proper debug, release, distribution builds.

		bool getAlert() { return hasAlert; }
		std::string& getAlertMessage() { return alertMessage; }
		CallSeverity getAlertSeverity() { return assetState; }

		void resetAlert();

		void resourceAsk(std::shared_ptr<ResourceRequest>& Request);
		
		virtual void askCallback(std::shared_ptr<ResourceAsk>& ask) {}
		
		template <typename T> 
		ResourceHandle<T> Request(size_t senderID, std::string resourceName, std::vector<size_t> resourceArgs);
		template <typename T> 
		ResourceHandle<T> Request(std::string senderName, std::string resourceName, std::vector<size_t> resourceArgs);

		template <typename T> 
		ResourceHandle<T> Request(size_t senderID, std::string resourceName)
		{
			return Request<T>(senderID, resourceName, {});
		}

		template <typename T> 
		ResourceHandle<T> Request(std::string senderName, std::string resourceName)
		{
			return Request<T>(senderName, resourceName, {});
		}

		virtual const std::string getAssetName() = 0;

		size_t getUUID() { return uuid; }

	protected:
		StarryAsset();
		StarryAsset(bool autoRegister);
		virtual void Alert(const std::string& message, CallSeverity severity);

		static size_t generateUUID();

		std::vector<std::shared_ptr<ResourceAsk>> asks;

	private:
		static std::mt19937_64 randomGen;

		bool hasAlert = false;
		CallSeverity assetState = NONE;
		std::string alertMessage = "";

		size_t uuid = 0;
	};
}