#pragma once

#include <memory>

#include "ITimer.hpp"

class ITimerFactory
{
public:
	virtual ~ITimerFactory() = default;

	/** create a single shot timer. This timer stops running when timeout is reached */
	virtual std::shared_ptr<ITimer> createSingleShotTimer() = 0;

	/** create a cyclic timer. This continues running when timeout is reached.
	 * It uses cycle time provided by start. */
	virtual std::shared_ptr<ITimer> createTickTimer() = 0;
};
