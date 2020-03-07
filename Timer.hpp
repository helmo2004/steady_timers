#pragma once
#include "ITimer.hpp"

class Timer : public ITimer
{
public:
	Timer(std::function<std::chrono::milliseconds(void)>, bool singleShot);

	void stop() override;

	void setTimeoutCallback(std::function<void()> callback) override;

	void start(std::chrono::milliseconds duration) override;

	bool expired() const override;

	bool isRunning() const override;

	std::chrono::milliseconds getRemainingMilliseconds() const override;

	friend class TimerManager;

private:
	std::function<void()> m_timeoutCallback = nullptr;
	std::function<std::chrono::milliseconds(void)> m_getTimeCallback = nullptr;
	bool m_running = false;
	bool m_expired = false;
	const bool m_isSingleShot = false;
	std::chrono::milliseconds m_expireTime = 0ms;
	std::chrono::milliseconds m_duration = 0ms;
};
