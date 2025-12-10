#pragma once

#include <chrono>

#include <StarryLog.h>

namespace Starry
{
	class Timer : StarryAsset {
		struct FrameMetric {
			uint64_t frameSamples = 0;
			uint64_t totalTime = 0;
			uint64_t timeSinceFlush = 0;
			bool isHot = false;
			const static int NANOS_IN_SECOND = 1'000'000'000;

			uint64_t framesPerSecond() {
				if (totalTime == 0) return 0;
				return static_cast<int>((frameSamples * NANOS_IN_SECOND) / totalTime);
			}
		};
		public:
			Timer();
			~Timer();
			void time();
			void stop();
			void end();

			void setLogging() {
				toLog = true;
			}

			float getDeltaTimeSeconds() const {
				return static_cast<float>(deltaTime) / static_cast<float>(FrameMetric::NANOS_IN_SECOND);
			}

			const std::string getAssetName() override { return "Timer"; };
		private:
			void logFPS();
			int getFPS();
			bool hasMetric() const {
				return frameMetric.timeSinceFlush >= LOG_UPDATE_TIME;
			}

			const static uint64_t LOG_UPDATE_TIME = FrameMetric::NANOS_IN_SECOND; // 1 second

			bool toLog = false;
			uint64_t currentTime = 0;
			uint64_t deltaTime = 0;
			FrameMetric frameMetric{};
			std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	};

}