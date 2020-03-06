#pragma once

#include "ITimerFactory.hpp"

class ITimerManager : public ITimerFactory
{
public:
	virtual void fastForward(std::chrono::milliseconds milliseconds) = 0;

	virtual void poll() = 0;
};
