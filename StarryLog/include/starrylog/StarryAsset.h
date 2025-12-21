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

namespace StarryLog
{
	class StarryAsset {
	public:
		virtual ~StarryAsset();

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
		// TODO: File and Not File
		// TODO: Add proper debug, release, distrubution builds.

		bool getAlert() { return hasAlert; }
		std::string& getAlertMessage() { return alertMessage; }
		CallSeverity getAlertSeverity() { return assetState; }

		void resetAlert();

		// Mabye a lifetimes enum

		virtual std::optional<void*> getResource(uint8_t resourceID) { return {}; };

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