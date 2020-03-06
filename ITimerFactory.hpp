#pragma once

#include <memory>
#include "ISingleShotTimer.hpp"

class ITimerFactory
{
public:
	virtual ~ITimerFactory() = default;

	/** create a single shot timer. This timer stops running when timeout is reached */
	virtual std::shared_ptr<ISingleShotTimer> createSingleShotTimer() = 0;
};
