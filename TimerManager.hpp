#pragma once

#include <chrono>
#include <functional>
#include <list>
#include "ITimerManager.hpp"

class SingleShotTimer;

extern std::chrono::milliseconds getChronoSteadyClockTicks(void);

class TimerManager : public ITimerManager
{
public:
	using SteadyTickCallbackType = std::function<std::chrono::milliseconds(void)>;

	TimerManager(SteadyTickCallbackType steadyTickProvider = getChronoSteadyClockTicks);

	std::shared_ptr<ISingleShotTimer> createSingleShotTimer() override;

	void fastForward(std::chrono::milliseconds milliseconds) override;

	void poll() override;

	void pause() override;

	void resume() override;

private:
	SteadyTickCallbackType m_steadyTickCallback;
	std::list<std::weak_ptr<SingleShotTimer>> m_timers;
	std::chrono::milliseconds m_pollTimeStamp = 0ms;
	std::chrono::milliseconds m_fastForwardOffset = 0ms;
	std::chrono::milliseconds m_pausingTime = 0ms;
	std::chrono::milliseconds m_pausingOffset = 0ms;
	bool m_paused = false;
	bool m_isCurrentlyPolling = false;
};
