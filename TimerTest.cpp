#include <gmock/gmock.h>
#include <chrono>
#include <thread>

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

	timer->start(1s);

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
	EXPECT_EQ(0ms, timer3->getRemainingMilliseconds());

	EXPECT_FALSE(timer1->isRunning());
	EXPECT_FALSE(timer2->isRunning());
	EXPECT_FALSE(timer3->isRunning());
	EXPECT_EQ(0ms, timer1->getRemainingMilliseconds());
	EXPECT_EQ(0ms, timer2->getRemainingMilliseconds());
	EXPECT_EQ(0ms, timer3->getRemainingMilliseconds());

	timer1->start(1100ms);
	EXPECT_TRUE(timer1->isRunning());
	EXPECT_FALSE(timer2->isRunning());
	EXPECT_FALSE(timer3->isRunning());
	EXPECT_EQ(1100ms, timer1->getRemainingMilliseconds());
	EXPECT_EQ(0ms, timer2->getRemainingMilliseconds());
	EXPECT_EQ(0ms, timer3->getRemainingMilliseconds());

	timer2->start(500ms);
	EXPECT_TRUE(timer1->isRunning());
	EXPECT_TRUE(timer2->isRunning());
	EXPECT_FALSE(timer3->isRunning());
	EXPECT_EQ(1100ms, timer1->getRemainingMilliseconds());
	EXPECT_EQ(500ms, timer2->getRemainingMilliseconds());
	EXPECT_EQ(0ms, timer3->getRemainingMilliseconds());

	Sequence seq;

	ON_CALL(timerCallback2, Call()).WillByDefault(
		Invoke([&]()
		{
			EXPECT_EQ(0ms, timer2->getRemainingMilliseconds());
			timer2->start(500ms);
		}));
	EXPECT_CALL(timerCallback2, Call()).InSequence(seq);
	EXPECT_CALL(timerCallback2, Call()).InSequence(seq); // Timer is re-started during expire callback
	EXPECT_CALL(timerCallback1, Call()).InSequence(seq);

	uut->fastForward(1100ms);

	EXPECT_EQ(0ms, timer3->getRemainingMilliseconds());
	EXPECT_EQ(400ms, timer2->getRemainingMilliseconds());
	EXPECT_EQ(0ms, timer1->getRemainingMilliseconds());

	EXPECT_FALSE(timer3->isRunning());
	EXPECT_TRUE(timer2->isRunning());
	EXPECT_FALSE(timer1->isRunning());
}

TEST_F(TimerTest, PauseAndFastForwardTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createSingleShotTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());
	EXPECT_FALSE(timer1->isRunning());
	timer1->start(1100ms);

	uut->pause();

	uut->fastForward(1000ms);
	m_currentTime += 1000ms; // this should not raise timer event since we are in paused mode
	uut->poll();
	EXPECT_EQ(100ms, timer1->getRemainingMilliseconds());
	EXPECT_CALL(timerCallback1, Call());
	uut->fastForward(100ms);
	EXPECT_EQ(0ms, timer1->getRemainingMilliseconds());

	EXPECT_CALL(timerCallback1, Call()).Times(0);
	uut->resume();
	timer1->start(500ms);
	m_currentTime += 499ms;
	uut->poll();
	EXPECT_EQ(1ms, timer1->getRemainingMilliseconds());

	m_currentTime += 1ms;
	EXPECT_CALL(timerCallback1, Call());
	uut->poll();
	EXPECT_EQ(0ms, timer1->getRemainingMilliseconds());
}

TEST_F(TimerTest, CycleTimerTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());
	EXPECT_FALSE(timer1->isRunning());
	EXPECT_FALSE(timer1->expired());
	timer1->start(250ms);

	EXPECT_CALL(timerCallback1, Call()).Times(3);
	m_currentTime += 999ms;
	uut->poll();
	EXPECT_EQ(1ms, timer1->getRemainingMilliseconds());
	EXPECT_TRUE(timer1->isRunning());
	EXPECT_FALSE(timer1->expired());

	EXPECT_CALL(timerCallback1, Call()).Times(1);
	m_currentTime += 1ms;
	uut->poll();
	EXPECT_EQ(250ms, timer1->getRemainingMilliseconds());
	EXPECT_TRUE(timer1->isRunning());
	EXPECT_FALSE(timer1->expired());
}

TEST_F(TimerTest, DontStartToShortDurationsForCycleTimerTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());


	timer1->start(0ms);
	EXPECT_FALSE(timer1->isRunning());
	m_currentTime += 1ms;
	uut->poll();

	timer1->start(0s);
	EXPECT_FALSE(timer1->isRunning());
	m_currentTime += 1ms;
	uut->poll();

	timer1->start(0min);
	EXPECT_FALSE(timer1->isRunning());
	m_currentTime += 1ms;
	uut->poll();

	timer1->start(0h);
	EXPECT_FALSE(timer1->isRunning());
	m_currentTime += 1ms;
	uut->poll();
}

TEST_F(TimerTest, ChronoSteadyClockTestTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = std::make_shared<TimerManager>();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	timer1->start(250ms);
	std::this_thread::sleep_for(250ms);
	EXPECT_CALL(timerCallback1, Call()).Times(1);
	uut->poll();
}

TEST_F(TimerTest, RemoveExpiredWeakPtrTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createSingleShotTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	timer1->start(500ms);
	EXPECT_CALL(timerCallback1, Call()).Times(1);
	uut->fastForward(500ms);

	timer1->start(500ms);
	EXPECT_CALL(timerCallback1, Call()).Times(0);
	timer1 = nullptr;
	uut->fastForward(500ms);
}

TEST_F(TimerTest, RemoveExpiredWeakPtrDirectlyAfterCallbackTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	timer1->start(500ms);
	EXPECT_CALL(timerCallback1, Call()).WillOnce(
		Invoke([&]()
		{
			timer1 = nullptr;
		}));
	m_currentTime += 1000ms;
	uut->poll();
}

TEST_F(TimerTest, NoPauseDuringPollTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	timer1->start(500ms);
	EXPECT_CALL(timerCallback1, Call()).WillOnce(
		Invoke([&]()
		{
			uut->pause();
		}));
	m_currentTime += 500ms;
	uut->poll();

	EXPECT_CALL(timerCallback1, Call()).Times(1);
	m_currentTime += 500ms;
	uut->poll();
	uut->poll();
}

TEST_F(TimerTest, NoResumeDuringPollTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	uut->pause();
	timer1->start(500ms);
	EXPECT_CALL(timerCallback1, Call()).WillOnce(
		Invoke([&]()
		{
			uut->resume();
		}));
	uut->fastForward(500ms); // polls indirectly in the end of time increment

	EXPECT_CALL(timerCallback1, Call()).Times(0);
	m_currentTime += 500ms;
	uut->poll();
	uut->poll();
}

TEST_F(TimerTest, NoFastForwardDuringPollTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	timer1->start(500ms);
	EXPECT_CALL(timerCallback1, Call()).WillOnce(
		Invoke([&]()
		{
			uut->fastForward(500ms); // this should trigger timer a second callback when we allow fast forward during callback
		}));
	m_currentTime += 500ms;
	uut->poll();
	uut->poll();
}

TEST_F(TimerTest, PollDuringDuringPollDoesNotDirsturbBehaviorTest)
{
	StrictMock<MockFunction<void(void)>> timerCallback1;

	auto uut = createUUT();

	auto timer1 = uut->createTickTimer();
	timer1->setTimeoutCallback(timerCallback1.AsStdFunction());

	timer1->start(500ms);
	EXPECT_CALL(timerCallback1, Call()).Times(4).WillRepeatedly(
		Invoke([&]()
		{
			uut->poll();
		}));
	m_currentTime += 2000ms;
	uut->poll();
}
