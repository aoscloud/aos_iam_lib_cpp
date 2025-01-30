/*
 * Copyright (C) 2023 Renesas Electronics Corporation.
 * Copyright (C) 2023 EPAM Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AOS_TIME_HPP_
#define AOS_TIME_HPP_

#include <time.h>

#include "aos/common/config.hpp"
#include "aos/common/tools/array.hpp"
#include "aos/common/tools/log.hpp"
#include "aos/common/tools/string.hpp"

namespace aos {

/**
 * Size of a time in string representation.
 */
const auto cTimeStrLen = AOS_CONFIG_TIME_STR_LEN;

/**
 * Base type for a time duration in nanoseconds. Can also be negative to set a point back in time.
 */
class Duration {
public:
    /**
     * Default constructor.
     */
    constexpr Duration() = default;

    // cppcheck-suppress noExplicitConstructor
    /**
     * Constructs object instance.
     *
     * @param duration duration in nanoseconds.
     */
    constexpr Duration(int64_t duration)
        : mDuration(duration)
    {
    }

    /**
     * Returns duration in nanoseconds.
     *
     * @result int64_t.
     */
    constexpr int64_t Count() const { return mDuration; }

    /**
     * Changes sign of the duration.
     *
     * @result Duration.
     */
    constexpr Duration operator-() const { return -mDuration; }

    /**
     * Compares two durations.
     *
     * @param obj duration to compare with.
     * @result bool.
     */
    bool operator==(const Duration& obj) const { return mDuration == obj.mDuration; }

    /**
     * Compares two durations.
     *
     * @param obj duration to compare with.
     * @result bool.
     */
    bool operator!=(const Duration& obj) const { return !operator==(obj); }

    /**
     * Compares two durations.
     *
     * @param obj duration to compare with.
     * @result bool.
     */
    bool operator<(const Duration& obj) const { return mDuration < obj.mDuration; }

    /**
     * Compares two durations.
     *
     * @param obj duration to compare with.
     * @result bool.
     */
    bool operator<=(const Duration& obj) const { return mDuration <= obj.mDuration; }

    /**
     * Compares two durations.
     *
     * @param obj duration to compare with.
     * @result bool.
     */
    bool operator>(const Duration& obj) const { return mDuration > obj.mDuration; }

    /**
     * Compares two durations.
     *
     * @param obj duration to compare with.
     * @result bool.
     */
    bool operator>=(const Duration& obj) const { return mDuration >= obj.mDuration; }

    /**
     * Returns ISO 8601 duration string representation.
     *
     * @return StaticString<cTimeStrLen>
     */
    StaticString<cTimeStrLen> ToISO8601String() const;

    /**
     * Logs duration.
     *
     * @param log log object.
     * @param duration duration.
     * @return Log&.
     */
    friend Log& operator<<(Log& log, const Duration& duration) { return log << duration.ToISO8601String(); }

private:
    int64_t mDuration = 0;
};

/**
 * Adds two durations and returns result as a new object.
 *
 * @param lhs left hand side duration.
 * @param rhs right hand side duration.
 * @result Duration.
 */
constexpr Duration operator+(const Duration& lhs, const Duration& rhs)
{
    return lhs.Count() + rhs.Count();
}

/**
 * Subtracts two durations and returns result as a new object.
 *
 * @param lhs left hand side duration.
 * @param rhs right hand side duration.
 * @result Duration.
 */
constexpr Duration operator-(const Duration& lhs, const Duration& rhs)
{
    return lhs.Count() - rhs.Count();
}

/**
 * Multiplies two durations and returns result as a new object.
 *
 * @param lhs left hand side duration.
 * @param rhs right hand side duration.
 * @result Duration.
 */
constexpr Duration operator*(const Duration& lhs, const Duration& rhs)
{
    return lhs.Count() * rhs.Count();
}

/**
 * Divides two durations and returns result as a new object.
 *
 * @param lhs left hand side duration.
 * @param rhs right hand side duration.
 * @result Duration.
 */
constexpr Duration operator/(const Duration& lhs, const Duration& rhs)
{
    return lhs.Count() / rhs.Count();
}

/**
 * An object specifying a time instant.
 */
class Time {
public:
    /**
     * Duration constants.
     */
    static constexpr Duration cNanoseconds  = 1;
    static constexpr Duration cMicroseconds = 1000 * cNanoseconds;
    static constexpr Duration cMilliseconds = 1000 * cMicroseconds;
    static constexpr Duration cSeconds      = 1000 * cMilliseconds;
    static constexpr Duration cMinutes      = 60 * cSeconds;
    static constexpr Duration cHours        = 60 * cMinutes;
    static constexpr Duration cDay          = 24 * cHours;
    static constexpr Duration cWeek         = 7 * cDay;
    static constexpr Duration cYear         = 31556925974740 * cMicroseconds;
    static constexpr Duration cMonth        = cYear / 12;

    /**
     * Constructs object instance
     */
    Time()
        : mTime()
    {
    }

    /**
     * Returns current local time.
     *
     * @param clockID clock ID: CLOCK_REALTIME or CLOCK_MONOTONIC.
     * @result Time.
     */
    static Time Now(clockid_t clockID = CLOCK_REALTIME)
    {
        timespec time;

        [[maybe_unused]] auto ret = clock_gettime(clockID, &time);
        assert(ret == 0);

        return Time(time);
    }

    /**
     * Returns local time corresponding to Unix time.
     *
     * @param sec seconds since January 1, 1970 UTC.
     * @param nsec nano seconds part.
     * @result Time.
     */
    static Time Unix(int64_t sec, int64_t nsec = 0)
    {
        timespec ts;

        ts.tv_sec  = sec;
        ts.tv_nsec = nsec;

        return Time(ts);
    }

    /**
     * Checks whether Time object is default initialized.
     *
     * @result bool.
     */
    bool IsZero() const { return *this == Time(); }

    /**
     * Returns a copy of the current object increased by a specified time duration.
     *
     * @param duration duration to be added to the current time instant.
     * @result Time.
     */
    Time Add(Duration duration) const
    {
        auto time = mTime;

        int64_t nsec = time.tv_nsec + duration.Count();

        time.tv_sec += nsec / cSeconds.Count();
        time.tv_nsec = nsec % cSeconds.Count();

        // sign of the remainder implementation defined => correct if negative
        if (time.tv_nsec < 0) {
            time.tv_nsec += cSeconds.Count();
            time.tv_sec--;
        }

        return Time(time);
    }

    /**
     * Returns duration as difference between two time points.
     *
     * @result Duration.
     */
    Duration Sub(const Time& time) const { return UnixNano() - time.UnixNano(); }

    /**
     * Returns unix time struct.
     *
     * @result unix time struct.
     */
    timespec UnixTime() const { return mTime; }

    /**
     * Returns an integer representing a time in nanoseconds elapsed since January 1, 1970 UTC.
     *
     * @result result time in nanoseconds.
     */
    uint64_t UnixNano() const
    {
        uint64_t nsec = mTime.tv_nsec + mTime.tv_sec * cSeconds.Count();

        return nsec;
    }

    /**
     * Returns date part of time.
     *
     * @param day day.
     * @param month month.
     * @param year year.
     * @return Error.
     */
    Error GetDate(int* day, int* month = nullptr, int* year = nullptr) const
    {
        tm timeInfo {};

        if (!gmtime_r(&mTime.tv_sec, &timeInfo)) {
            return ErrorEnum::eNoMemory;
        }

        *day = timeInfo.tm_mday;

        if (month) {
            *month = timeInfo.tm_mon + 1;
        }

        if (year) {
            *year = timeInfo.tm_year + 1900;
        }

        return ErrorEnum::eNone;
    }

    /**
     * Returns time part of time.
     *
     * @param hour hour.
     * @param min min.
     * @param sec sec.
     * @return Error.
     */
    Error GetTime(int* hour, int* min = nullptr, int* sec = nullptr) const
    {
        tm timeInfo {};

        if (!gmtime_r(&mTime.tv_sec, &timeInfo)) {
            return ErrorEnum::eNoMemory;
        }

        *hour = timeInfo.tm_hour;

        if (min) {
            *min = timeInfo.tm_min;
        }

        if (sec) {
            *sec = timeInfo.tm_sec;
        }

        return ErrorEnum::eNone;
    }

    /**
     * Checks whether a current time is less than a specified one.
     *
     * @param obj time object to compare with.
     * @result bool.
     */
    bool operator<(const Time& obj) const
    {
        return mTime.tv_sec < obj.mTime.tv_sec
            || (mTime.tv_sec == obj.mTime.tv_sec && mTime.tv_nsec < obj.mTime.tv_nsec);
    }

    /**
     * Checks whether a current time and a specified object represent the same time instant.
     *
     * @param obj time object to compare with.
     * @result bool.
     */
    bool operator==(const Time& obj) const
    {
        return mTime.tv_sec == obj.mTime.tv_sec && mTime.tv_nsec == obj.mTime.tv_nsec;
    }

    /**
     * Checks whether a current time and a specified object don't represent the same time instant.
     *
     * @param obj time object to compare with.
     * @result bool.
     */
    bool operator!=(const Time& obj) const { return !operator==(obj); }

    /**
     * Prints time into log.
     *
     * @param log log object to print time into.
     * @param obj time object to be printed.
     * @result source stream object.
     */
    friend Log& operator<<(Log& log, const Time& obj)
    {
        tm                        buf;
        StaticString<cTimeStrLen> utcTimeStr;

        auto time = gmtime_r(&obj.mTime.tv_sec, &buf);
        assert(time != nullptr);

        utcTimeStr.Resize(utcTimeStr.MaxSize());

        size_t size = strftime(utcTimeStr.Get(), utcTimeStr.Size(), "%FT%TZ", time);
        assert(size != 0);

        utcTimeStr.Resize(size - 1);

        log << utcTimeStr;

        return log;
    }

private:
    explicit Time(timespec time)
        : mTime(time)
    {
    }

    timespec mTime;
};

/**
 * Returns time duration in years.
 *
 * @param num number of years.
 * @result Duration.
 */
constexpr Duration Years(int64_t num)
{
    return Time::cYear * num;
}

} // namespace aos

#endif
