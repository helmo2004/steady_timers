#pragma once
#include "ISingleShotTimer.hpp"

class SingleShotTimer : public ISingleShotTimer
{
public:
	SingleShotTimer(std::function<std::chrono::milliseconds(void)>);

	void stop() override;

	void setTimeoutCallback(std::function<void()> callback) override;

	void start(std::chrono::milliseconds durationMs) override;

	bool expired() const override;

	std::chrono::milliseconds getRemainingMilliseconds() const override;

	friend class TimerManager;

private:
	std::function<void()> m_timeoutCallback = nullptr;
	std::function<std::chrono::milliseconds(void)> m_getTimeCallback = nullptr;
	bool m_running = false;
	bool m_expired = false;
	std::chrono::milliseconds m_expireTime;
};
