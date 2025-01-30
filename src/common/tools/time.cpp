/*
 * Copyright (C) 2025 EPAM Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "aos/common/tools/time.hpp"

namespace aos {

/***********************************************************************************************************************
 * Public
 **********************************************************************************************************************/

StaticString<cTimeStrLen> Duration::ToISO8601String() const
{
    if (mDuration == 0) {
        return "PT0S";
    }

    StaticString<cTimeStrLen> result = (mDuration < 0) ? "-P" : "P";

    auto total = abs(mDuration);
    char buffer[16];

    if (auto years = total / Time::cYear.Count(); years > 0) {
        snprintf(buffer, sizeof(buffer), "%ldY", years);

        result.Append(buffer);

        total %= Time::cYear.Count();
    }

    if (auto months = total / Time::cMonth.Count(); months > 0) {
        snprintf(buffer, sizeof(buffer), "%ldM", months);

        result.Append(buffer);
        total %= Time::cMonth.Count();
    }

    if (auto weeks = total / Time::cWeek.Count(); weeks > 0) {
        snprintf(buffer, sizeof(buffer), "%ldW", weeks);

        result.Append(buffer);

        total %= Time::cWeek.Count();
    }

    if (auto days = total / Time::cDay.Count(); days > 0) {
        snprintf(buffer, sizeof(buffer), "%ldD", days);

        result.Append(buffer);

        total %= Time::cDay.Count();
    }

    const auto hours = total / Time::cHours.Count();
    total %= Time::cHours.Count();

    const auto minutes = total / Time::cMinutes.Count();
    total %= Time::cMinutes.Count();

    auto seconds = total / Time::cSeconds.Count();
    total %= Time::cSeconds.Count();

    if (hours || minutes || seconds || total) {
        result.Append("T");

        if (hours) {
            snprintf(buffer, sizeof(buffer), "%ldH", hours);
            result.Append(buffer);
        }

        if (minutes) {
            snprintf(buffer, sizeof(buffer), "%ldM", minutes);
            result.Append(buffer);
        }

        if (total == 0 && seconds > 0) {
            snprintf(buffer, sizeof(buffer), "%ldS", seconds);
            result.Append(buffer);
        }

        if (total > 0) {
            const auto rest = static_cast<double>(total) / Time::cSeconds.Count() + static_cast<double>(seconds);

            snprintf(buffer, sizeof(buffer), "%0.9lfS", rest);
            result.Append(buffer);
        }
    }

    return result;
}

} // namespace aos
