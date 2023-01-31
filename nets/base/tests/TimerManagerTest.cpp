// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Test for TimerManager

#include <gtest/gtest.h>

#include "nets/base/timer/TimerManager.h"

using namespace nets;

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
    timerManager->addTimer(Timestamp::now().plusSeconds(2), 1, 1000, false, testFunc);
    ::sleep(2);
    timerManager->update();
}

TEST_F(TimerManagerTest, ExecuteRepeatable)
{
    timerManager->addTimer(Timestamp::now().plusSeconds(1), Timer::kRepeatForever, 2000, false, testFunc);
    ::sleep(1);
    for (Int32Type i = 0; i < 6; ++i)
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
    timerManager->addTimer(Timestamp::now().plusMilliseconds(2000), 6, 3000, true, fixedDelayFunc);
    ::sleep(2);
    for (Int32Type i = 0; i < 6; ++i)
    {
        timerManager->update();
        ::sleep(3);
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
    timerManager->addTimer(Timestamp::now().plusMilliseconds(1000), Timer::kRepeatForever, 2000, false, func1);
    TimerId timerId2 =
        timerManager->addTimer(Timestamp::now().plusMilliseconds(1000), Timer::kRepeatForever, 2000, false, func2);
    ::sleep(1);
    for (Int32Type i = 0; i < 6; ++i)
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