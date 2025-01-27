#pragma once

#include "ITimerManager.hpp"
#include <chrono>
#include <functional>
#include <list>

class Timer;

extern std::chrono::milliseconds getChronoSteadyClockTicks(void);

class TimerManager : public ITimerManager
{
public:
    using SteadyTickCallbackType = std::function<std::chrono::milliseconds(void)>;

    TimerManager(SteadyTickCallbackType steadyTickProvider = getChronoSteadyClockTicks);

    ~TimerManager();

    std::shared_ptr<ITimer> createSingleShotTimer() override;

    std::shared_ptr<ITimer> createTickTimer() override;

    void fastForward(std::chrono::milliseconds milliseconds) override;

    void poll() override;

    void pause() override;

    void resume() override;

private:
    TimerManager(const TimerManager&) = delete;
    TimerManager(TimerManager&&) = delete;

    SteadyTickCallbackType m_steadyTickCallback;
    SteadyTickCallbackType m_originalSteadyTickCallback; // needed to restore
    std::list<std::weak_ptr<Timer>> m_timers;
    std::chrono::milliseconds m_pollTimeStamp = 0ms;
    std::chrono::milliseconds m_fastForwardOffset = 0ms;
    std::chrono::milliseconds m_pausingTime = 0ms;
    std::chrono::milliseconds m_pausingOffset = 0ms;
    bool m_paused = false;
    bool m_isCurrentlyPolling = false;
};
