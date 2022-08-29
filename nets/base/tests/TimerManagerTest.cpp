//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/timer/TimerManager.h"

using namespace nets::base;

using TimerId = Timer::TimerId;

class TimerManagerTest : public testing::Test
{
public:
    void SetUp() override
    {
        timerManager = new TimerManager();
    }
    void TearDown() override
    {
        delete timerManager;
    }

protected:
    TimerManager* timerManager {nullptr};
};

void testFunc()
{
    ::printf("execute testFunc\n");
    ::fflush(stdout);
}

TEST_F(TimerManagerTest, ExecuteNonRepeatable)
{
    timerManager->addNonRepeatableTimer(Timestamp::now().plusSeconds(2), testFunc);
    while (1)
    {
        ::sleep(2);
        timerManager->update();
    }
}

TEST_F(TimerManagerTest, ExecuteRepeatable)
{
    timerManager->addFixedRateTimer(Timestamp::now().plusMilliseconds(1000), 2000, testFunc);
    ::sleep(1);
    for (::int32_t i = 0; i < 6; ++i)
    {
        timerManager->update();
        ::sleep(2);
    }
}

void fixedDelayFunc()
{
    ::printf("execute fixedDelayFunc start\n");
    ::fflush(stdout);
    ::sleep(2);
    ::printf("execute fixedDelayFunc end\n");
    ::fflush(stdout);
}

TEST_F(TimerManagerTest, ExecuteFixedDelay)
{
    timerManager->addFixedDelayTimer(Timestamp::now().plusMilliseconds(2000), 3000, fixedDelayFunc);
    ::sleep(2);
    for (::int32_t i = 0; i < 6; ++i)
    {
        timerManager->update();
        ::sleep(3);
    }
}

TEST_F(TimerManagerTest, ExecuteLimitRepeatable)
{
    timerManager->addTimer(Timestamp::now().plusMilliseconds(2000), 3, 2000, false, testFunc);
    while (1)
    {
        ::sleep(2);
        timerManager->update();
    }
}

void func1()
{
    ::printf("execute func1\n");
    ::fflush(stdout);
}

void func2()
{
    ::printf("execute func2\n");
    ::fflush(stdout);
}

TEST_F(TimerManagerTest, Remove)
{
    timerManager->addFixedRateTimer(Timestamp::now().plusMilliseconds(1000), 2000, func1);
    TimerId timerId2 = timerManager->addFixedRateTimer(Timestamp::now().plusMilliseconds(1000), 2000, func2);
    ::sleep(1);
    for (::int32_t i = 0; i < 6; ++i)
    {
        timerManager->update();
        if (i == 3)
        {
            timerManager->removeTimer(timerId2);
        }
        ::sleep(2);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}