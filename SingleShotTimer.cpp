#include "SingleShotTimer.hpp"

SingleShotTimer::SingleShotTimer(std::function<std::chrono::milliseconds(void)> callback)
: m_getTimeCallback(callback)
{}

void SingleShotTimer::stop()
{
	if (m_running)
	{
		m_running = false;
	}
}

void SingleShotTimer::setTimeoutCallback(std::function<void()> callback)
{
	m_timeoutCallback = callback;
}

void SingleShotTimer::start(std::chrono::milliseconds durationMs)
{
	if (m_running)
	{
		return;
	}
	m_running = true;
	m_expireTime = m_getTimeCallback() + durationMs;
}

bool SingleShotTimer::expired() const
{
	return m_expired;
}

std::chrono::milliseconds SingleShotTimer::getRemainingMilliseconds() const
{
	return m_expireTime - m_getTimeCallback();
}
