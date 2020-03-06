#include <chrono>
#include <iostream>

#include "SingleShotTimer.hpp"
#include "TimerManager.hpp"

std::chrono::milliseconds getChronoSteadyClockTicks(void)
{
	auto now = std::chrono::steady_clock::now(); // should be nanoseconds -> cast to milliseconds
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	return millis;
}

TimerManager::TimerManager(SteadyTickCallbackType steadyTickProvider)
{
	m_steadyTickCallback = [&, steadyTickProvider]()
	{
		std::chrono::milliseconds result;
		if (m_isCurrentlyPolling)
		{
			result = m_pollTimeStamp;
		}
		else
		{
			result = steadyTickProvider() + m_fastForwardOffset;
		}
		return result;
	};
}


std::shared_ptr<ISingleShotTimer> TimerManager::createSingleShotTimer()
{
	auto timer = std::make_shared<SingleShotTimer>(m_steadyTickCallback);
	m_timers.push_back(timer);
	return timer;
}

void TimerManager::fastForward(std::chrono::milliseconds milliseconds)
{
	m_fastForwardOffset += milliseconds;
	poll();
}

void TimerManager::poll()
{
	const auto currentTime = m_steadyTickCallback();
	// this flag allows time duration correct timer behavior when timers are created during poll in callback
	// we modify the current time to the time of currently expired timer
	m_isCurrentlyPolling = true;

	auto getNextExpiredTimer = [&]()
	{
		std::shared_ptr<SingleShotTimer> result;
		auto timerIterator = m_timers.begin();
		while (timerIterator != m_timers.end())
		{
			if (auto lockedTimer = timerIterator->lock())
			{
				if (lockedTimer->m_running)
				{
					if (currentTime >= lockedTimer->m_expireTime)
					{
						// take first found expired timer and later take earlier expired timer if found
						if ((result == nullptr) or (result->m_expireTime > lockedTimer->m_expireTime))
						{
							result = lockedTimer;
						}
					}
				}
				++timerIterator;
			}
			else
			{
				// cleanup list when shared ptr is deleted
				timerIterator = m_timers.erase(timerIterator);
			}
		}
		return result;
	};

	// Process next expired timer, then determine next expired timer again
	while (auto timer = getNextExpiredTimer())
	{
		m_pollTimeStamp = timer->m_expireTime;
		timer->stop();
		if (timer->m_timeoutCallback)
		{
			timer->m_timeoutCallback();
		}
	}
	m_isCurrentlyPolling = false;
}


