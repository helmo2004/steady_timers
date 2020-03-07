#pragma once

#include "ITimerFactory.hpp"

class ITimerManager : public ITimerFactory
{
public:
	/** Needs to be cyclically called in order to raise events for created timers.
	 * This raises all timer events and takes care of all expired timers in that duration,
	 * even when they are started during timeout-callbacks*/
	virtual void poll() = 0;

	/** Manipulation for test: fast forward. This raises all timer events and takes care of all expired timers in that duration,
	 * even when they are started during timeout-callbacks */
	virtual void fastForward(std::chrono::milliseconds milliseconds) = 0;

	/** stop timers. Only fast forward can expire timers from now on. */
	virtual void pause() = 0;

	/** restart timers. */
	virtual void resume() = 0;
};
