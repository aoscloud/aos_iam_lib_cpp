/*
 * Copyright (C) 2023 Renesas Electronics Corporation.
 * Copyright (C) 2023 EPAM Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gmock/gmock.h>

#include "aos/common/tools/time.hpp"
#include "aos/test/log.hpp"

using namespace testing;
using namespace aos;

class TimeTest : public Test {
private:
    void SetUp() override { test::InitLog(); }
};

TEST_F(TimeTest, DurationToISO8601String)
{
    struct TestCase {
        Duration    duration;
        const char* expected;
    } testCases[] = {
        {-6 * Time::cDay, "-P6D"},
        {Time::cWeek, "P1W"},
        {2 * Time::cWeek, "P2W"},
        {Time::cWeek - Time::cDay, "P6D"},
        {Time::cMonth, "P1M"},
        {Time::cYear, "P1Y"},
        {Time::cYear + Time::cMonth + Time::cWeek + Time::cDay + Time::cHours, "P1Y1M1W1DT1H"},
        {Duration(0), "PT0S"},
        {Duration(1), "PT0.000000001S"},
        {Time::cMinutes + Time::cSeconds, "PT1M1S"},
        {Time::cMinutes + 32 * Time::cMicroseconds, "PT1M0.000032000S"},
    };

    for (const auto& testCase : testCases) {
        LOG_DBG() << "Duration: " << testCase.duration;

        EXPECT_STREQ(testCase.duration.ToISO8601String().CStr(), testCase.expected);
    }
}

TEST_F(TimeTest, Add4Years)
{
    Time now             = Time::Now();
    Time fourYearsLater  = now.Add(Years(4));
    Time fourYearsBefore = now.Add(Years(-4));

    LOG_INF() << "Time now: " << now;
    LOG_INF() << "Four years later: " << fourYearsLater;

    EXPECT_EQ(now.UnixNano() + Years(4).Count(), fourYearsLater.UnixNano());
    EXPECT_EQ(now.UnixNano() + Years(-4).Count(), fourYearsBefore.UnixNano());
}

TEST_F(TimeTest, Compare)
{
    auto now = Time::Now();

    const Duration year       = Years(1);
    const Duration oneNanosec = 1;

    EXPECT_TRUE(now < now.Add(year));
    EXPECT_TRUE(now < now.Add(oneNanosec));

    EXPECT_FALSE(now.Add(oneNanosec) < now);
    EXPECT_FALSE(now < now);
}

TEST_F(TimeTest, GetDateTime)
{
    auto t = Time::Unix(1706702400);

    int day, month, year, hour, min, sec;

    EXPECT_TRUE(t.GetDate(&day, &month, &year).IsNone());
    EXPECT_TRUE(t.GetTime(&hour, &min, &sec).IsNone());

    EXPECT_EQ(day, 31);
    EXPECT_EQ(month, 1);
    EXPECT_EQ(year, 2024);
    EXPECT_EQ(hour, 12);
    EXPECT_EQ(min, 00);
    EXPECT_EQ(sec, 00);
}
