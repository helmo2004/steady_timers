#pragma once
#include <chrono>
#include <functional>

#include "ChronoHelpers.hpp"

// allows easy to use durations h, min, s, ms, us, ns
// timer.start(2min)
using namespace std::chrono_literals;

class ITimer
{
public:
	virtual ~ITimer() = default;

	virtual void stop() = 0;

	virtual void setTimeoutCallback(std::function<void()> callback) = 0;

	virtual void start(std::chrono::milliseconds duration) = 0;

	virtual bool expired() const = 0;

	virtual bool isRunning() const = 0;

	virtual std::chrono::milliseconds getRemainingMilliseconds() const = 0;
};
