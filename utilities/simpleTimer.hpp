#pragma once

#include <chrono>

// simple mcs chrono timer
class TimeCount
{
    using steady_clock = std::chrono::steady_clock;
    using microseconds = std::chrono::microseconds;
public:
    explicit TimeCount()
    {
        start();
    }
    void start()
    {
        m_Start = steady_clock::now();
    }

    void end()
    {
        m_End = steady_clock::now();
    }

    microseconds elapsed() const
    {
        return std::chrono::duration_cast<microseconds>(m_End - m_Start);
    }

    double getSec() const
    {
        double timePassed = static_cast<double>(elapsed().count());
        timePassed *= 1e-6;
        return timePassed;
    }
private:
    steady_clock::time_point m_Start;
    steady_clock::time_point m_End;
};