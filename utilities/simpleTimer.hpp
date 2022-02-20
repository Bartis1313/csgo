#pragma once
#include <chrono>
#include "utilities.hpp"

// simple ms chrono timer
class TimeCount
{
    using high_resolution_clock = std::chrono::high_resolution_clock;
    using microsecond = std::chrono::microseconds;
public:
    explicit TimeCount()
    {
        start();
    }
    void start()
    {
        m_Start = high_resolution_clock::now();
    }

    void end()
    {
        m_End = high_resolution_clock::now();
    }

    microsecond elapsed() const
    {
        return std::chrono::duration_cast<microsecond>(m_End - m_Start);
    }

    double getSec() const
    {
        double timePassed = elapsed().count();
        timePassed *= 1e-6;
        return timePassed;
    }
private:
    high_resolution_clock::time_point m_Start;
    high_resolution_clock::time_point m_End;
} inline timer;

#define START_TIME timer.start();
#define END_TIME timer.end();
// str is desc, for example str - hooks took: 
// the time in ms is added at the end
// no need to pass string xored
#define PRINT_TIME_TO_LOG(str) \
    LOG(LOG_INFO, std::format(XOR("{} {:.5f}ms"), str, timer.getSec()));