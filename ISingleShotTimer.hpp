#pragma once
#include <chrono>
#include <functional>

#include "ChronoHelpers.hpp"

using namespace std::chrono_literals;

class ISingleShotTimer
{
public:
	virtual ~ISingleShotTimer() = default;

	virtual void stop() = 0;

	virtual void setTimeoutCallback(std::function<void()> callback) = 0;

	virtual void start(std::chrono::milliseconds durationMs) = 0;

	virtual bool expired() const = 0;

	virtual std::chrono::milliseconds getRemainingMilliseconds() const = 0;
};
