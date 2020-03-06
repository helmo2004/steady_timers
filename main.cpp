#include <iostream>
#include <thread>

#include <gmock/gmock.h>

#include "TimerManager.hpp"


int main(void)
{
	std::shared_ptr<ITimerManager> tm = std::make_shared<TimerManager>();

	auto t = tm->createSingleShotTimer();

	t->start(250ms);
	t->setTimeoutCallback([t](){t->start(250ms); std::cout << "expired" << std::endl; });

	tm->fastForward(1000ms);
	std::this_thread::sleep_for(1000ms);
	tm->poll();
	std::cout << "Foo" << std::endl;
	return 0;
}
