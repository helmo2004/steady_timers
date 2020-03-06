#include "Timer.hpp"

Timer::Timer(std::function<std::chrono::milliseconds(void)> callback, bool singleShot)
: m_getTimeCallback(callback)
, m_isSingleShot(singleShot)
{}

void Timer::stop()
{
	if (m_running)
	{
		m_running = false;
	}
}

bool Timer::isRunning() const
{
	return m_running;
}

void Timer::setTimeoutCallback(std::function<void()> callback)
{
	m_timeoutCallback = callback;
}

void Timer::start(std::chrono::milliseconds duration)
{
	if (m_running or duration == 0ms)
	{
		return;
	}
	m_duration = duration;
	m_running = true;
	m_expireTime = m_getTimeCallback() + duration;
}

bool Timer::expired() const
{
	return m_expired;
}

std::chrono::milliseconds Timer::getRemainingMilliseconds() const
{
	if (m_running)
	{
		return m_expireTime - m_getTimeCallback();
	}
	else
	{
		return 0ms;
	}
}
