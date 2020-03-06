#pragma once

#include <memory>
#include "ISingleShotTimer.hpp"

class ITimerFactory
{
public:
	virtual ~ITimerFactory() = default;

	virtual std::shared_ptr<ISingleShotTimer> createSingleShotTimer() = 0;
};
