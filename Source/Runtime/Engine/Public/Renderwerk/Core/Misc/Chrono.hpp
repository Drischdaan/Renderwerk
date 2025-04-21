#pragma once

#include <chrono>

template <typename T, typename TPeriod= std::ratio<1>>
using TDuration = std::chrono::duration<T, TPeriod>;

using FNanosecondsDuration = std::chrono::nanoseconds;
using FMillisecondsDuration = std::chrono::milliseconds;
using FSecondsDuration = std::chrono::seconds;
using FMinutesDuration = std::chrono::minutes;
using FHoursDuration = std::chrono::hours;
using FMonthsDuration = std::chrono::months;
using FYearsDuration = std::chrono::years;
