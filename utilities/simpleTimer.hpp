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
};