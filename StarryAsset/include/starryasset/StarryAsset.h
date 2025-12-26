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
#include <typeindex>

namespace StarryAssets
{
	template <typename T>
	class ResourceHandle;

	class StarryAsset {
	public:
		virtual ~StarryAsset();

		StarryAsset(const StarryAsset& other) = delete;
		StarryAsset& operator=(const StarryAsset& other) = delete;
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
		// TODO: File and Not File
		// TODO: Add proper debug, release, distribution builds.

		bool getAlert() { return hasAlert; }
		std::string& getAlertMessage() { return alertMessage; }
		CallSeverity getAlertSeverity() { return assetState; }

		void resetAlert();

		virtual std::optional<void*> getResource(size_t resourceID, const std::type_index& typeInfo) { return {}; }
		virtual size_t getResourceIDFromString(std::string resourceName) { return -1; }
		
		template <typename T>
    	ResourceHandle<T> requestResource(uint64_t senderID, size_t resourceID);

		template <typename T>
    	ResourceHandle<T> requestResource(uint64_t senderID, std::string resourceName);

		template <typename T>
    	ResourceHandle<T> requestResource(std::string senderName, size_t resourceID);

		template <typename T>
    	ResourceHandle<T> requestResource(std::string senderName, std::string resourceName);

		virtual const std::string getAssetName() = 0;

		uint64_t getUUID() { return uuid; }

	protected:
		StarryAsset();
		StarryAsset(bool autoRegister);
		virtual void registerAlert(const std::string& message, CallSeverity severity);

	private:
		static uint64_t generateUUID();
		static std::mt19937_64 randomGen;

		bool hasAlert = false;
		CallSeverity assetState = NONE;
		std::string alertMessage = "";

		uint64_t uuid = 0;
	};
}