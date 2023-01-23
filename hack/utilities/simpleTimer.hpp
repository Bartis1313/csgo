#pragma once

#include <chrono>

struct TimeCount
{
    using steady_clock = std::chrono::steady_clock;
public:
    explicit TimeCount()
        : m_Start{ steady_clock::now() }
    {}
   
    void end() { m_End = steady_clock::now(); }
    constexpr auto elapsed() const { return m_End - m_Start; }
    template<typename T = float, typename period = std::chrono::seconds::period>
    constexpr auto getTime() const { return std::chrono::duration<T, period>(this->elapsed()).count(); }
private:
    steady_clock::time_point m_Start;
    steady_clock::time_point m_End;
};