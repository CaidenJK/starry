#include "Timer.h"

#include <string>

namespace StarryRender {
	Timer::Timer() {
		startTime = std::chrono::high_resolution_clock::now();
		deltaTime = startTime.time_since_epoch().count();
	}
	Timer::~Timer() {
		stop();
	}
	void Timer::time() {
		auto now_tp = std::chrono::high_resolution_clock::now();
		uint64_t now = static_cast<uint64_t>(now_tp.time_since_epoch().count());
		if (frameMetric.isHot == false) {
			frameMetric.isHot = true;
		}
		else {
			frameMetric.totalTime += (now - deltaTime);
			frameMetric.frameSamples++;
		}
		deltaTime = now;
		logFPS();
	}
	void Timer::stop() {
		deltaTime = 0;
		frameMetric.totalTime = 0;
		frameMetric.frameSamples = 0;
		frameMetric.isHot = false;
	}
	void Timer::logFPS() {
		if (!toLog || !hasMetric()) { return; }

		registerAlert("Current FPS: " + std::to_string(getFPS()), INFO_URGANT);
	}
	int Timer::getFPS() {
		if (frameMetric.isHot == false) {
			return -1;
		}
		return static_cast<int>(frameMetric.framesPerSecond());
	}
}