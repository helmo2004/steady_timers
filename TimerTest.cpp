#include <gmock/gmock.h>
#include <chrono>

#include "TimerManager.hpp"

using namespace ::testing;

class TimerTest : public Test
{
public:
	TimerTest()
	{
		EXPECT_CALL(m_getTimeCallback, Call()).WillRepeatedly(ReturnPointee(&m_currentTime));
	}

	std::shared_ptr<ITimerManager> createUUT()
	{

		return std::make_shared<TimerManager>(m_getTimeCallback.AsStdFunction());
	}

	MockFunction<std::chrono::milliseconds(void)> m_getTimeCallback;
	std::chrono::milliseconds m_currentTime = 0ms;
};

TEST_F(TimerTest, simpleExpireTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback;
	auto uut = createUUT();

	auto timer = uut->createSingleShotTimer();
	timer->setTimeoutCallback(timerCallback.AsStdFunction());

	timer->start(1000ms);

	uut->poll();
	m_currentTime += 999ms;
	uut->poll();
	m_currentTime += 1ms;
	EXPECT_CALL(timerCallback, Call());
	uut->poll();
}


TEST_F(TimerTest, MultipleTimersExpireInRightOrderTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;
	StrictMock<MockFunction<void(void)>> timerCallback2;
	auto uut = createUUT();

	auto timer1 = uut->createSingleShotTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());
	auto timer2 = uut->createSingleShotTimer();
	timer2->setTimeoutCallback(timerCallback2.AsStdFunction());

	auto timer3 = uut->createSingleShotTimer();

	timer1->start(1100ms);
	timer2->start(500ms);

	Sequence seq;

	ON_CALL(timerCallback2, Call()).WillByDefault(Invoke(
		[&]()
		{
			timer2->start(500ms);
		}));
	EXPECT_CALL(timerCallback2, Call()).InSequence(seq);
	EXPECT_CALL(timerCallback2, Call()).InSequence(seq); // Timer is re-started during expire callback
	EXPECT_CALL(timerCallback1, Call()).InSequence(seq);

	m_currentTime += 1100ms;
	uut->poll();
}

TEST_F(TimerTest, MultipleTimersExpireInRightOrderFastForwardTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;
	StrictMock<MockFunction<void(void)>> timerCallback2;
	auto uut = createUUT();

	auto timer1 = uut->createSingleShotTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());
	auto timer2 = uut->createSingleShotTimer();
	timer2->setTimeoutCallback(timerCallback2.AsStdFunction());

	auto timer3 = uut->createSingleShotTimer();

	timer1->start(1100ms);
	timer2->start(500ms);

	Sequence seq;

	ON_CALL(timerCallback2, Call()).WillByDefault(
		Invoke([&]()
		{
			timer2->start(500ms);
		}));
	EXPECT_CALL(timerCallback2, Call()).InSequence(seq);
	EXPECT_CALL(timerCallback2, Call()).InSequence(seq); // Timer is re-started during expire callback
	EXPECT_CALL(timerCallback1, Call()).InSequence(seq);

	uut->fastForward(1100ms);
}

TEST_F(TimerTest, PauseAndFastForwardTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createSingleShotTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	timer1->start(1100ms);

	uut->pause();

	uut->fastForward(1000ms);
	m_currentTime += 1000ms; // this should not raise timer event since we are in paused mode
	uut->poll();
	EXPECT_CALL(timerCallback1, Call());
	uut->fastForward(100ms);

	EXPECT_CALL(timerCallback1, Call()).Times(0);
	uut->resume();
	timer1->start(500ms);
	m_currentTime += 499ms;
	uut->poll();
	m_currentTime += 1ms;
	EXPECT_CALL(timerCallback1, Call());
	uut->poll();

}
