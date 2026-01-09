#include "Timer.h"

#include <string>

namespace Starry
{
	Timer::Timer() 
	{
		startTime = std::chrono::high_resolution_clock::now();
		currentTime = startTime.time_since_epoch().count();
	}
	Timer::~Timer() 
	{
		end();
	}
	void Timer::time() 
	{
		auto now_tp = std::chrono::high_resolution_clock::now();
		uint64_t now = static_cast<uint64_t>(now_tp.time_since_epoch().count());

		uint64_t delta = now - currentTime;

		if (frameMetric.isHot == false) {
			frameMetric.isHot = true;
		}
		else {
			frameMetric.totalTime += delta;
			frameMetric.frameSamples++;
			frameMetric.timeSinceFlush += delta;
		}
		currentTime = now;
		deltaTime = delta;
		if (frameMetric.isHot) { logFPS(); }
	}
	void Timer::stop() 
	{
		time();
		frameMetric.isHot = false;
	}
	void Timer::end() 
	{
		currentTime = 0;
		frameMetric.totalTime = 0;
		frameMetric.frameSamples = 0;
		frameMetric.isHot = false;
	}
	void Timer::logFPS() 
	{
		if (!toLog || !hasMetric()) { return; }

		frameMetric.timeSinceFlush = 0;
		Alert("Current FPS: " + std::to_string(getFPS()), INFO_URGANT);
	}
	int Timer::getFPS() 
	{
		if (frameMetric.isHot == false) {
			return -1;
		}
		return static_cast<int>(frameMetric.framesPerSecond());
	}
}