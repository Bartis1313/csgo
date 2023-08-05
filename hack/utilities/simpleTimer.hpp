#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <ctime>
#endif

enum class TimeUnit
{
    Seconds,
    Milliseconds,
    Microseconds
};

template <TimeUnit Unit = TimeUnit::Seconds>
class TimeCount
{
public:
    TimeCount()
    {
#ifdef _WIN32
        QueryPerformanceFrequency(&m_Frequency);
        QueryPerformanceCounter(&m_Start);
#else
        clock_gettime(CLOCK_MONOTONIC, &m_Start);
#endif
    }

    void end()
    {
#ifdef _WIN32
        QueryPerformanceCounter(&m_End);
#else
        clock_gettime(CLOCK_MONOTONIC, &m_End);
#endif
    }

    double elapsed() const
    {
#ifdef _WIN32
        return static_cast<double>(m_End.QuadPart - m_Start.QuadPart) / m_Frequency.QuadPart;
#else
        return static_cast<double>(m_End.tv_sec - m_Start.tv_sec) +
            static_cast<double>(m_End.tv_nsec - m_Start.tv_nsec) / 1e9;
#endif
    }

    template <TimeUnit U = Unit>
    auto getTime() const
    {
        if constexpr (U == TimeUnit::Seconds)
            return elapsed();
        else if constexpr (U == TimeUnit::Milliseconds)
            return elapsed() * 1000;
        else if constexpr (U == TimeUnit::Microseconds)
            return elapsed() * 1e6;
    }

private:
#ifdef _WIN32
    LARGE_INTEGER m_Start;
    LARGE_INTEGER m_End;
    LARGE_INTEGER m_Frequency;
#else
    timespec m_Start;
    timespec m_End;
#endif
};