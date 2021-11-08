#pragma once
#include "../dependencies/lazy_importer.hpp"
#include "../dependencies/xorstr.hpp"
#include "console/console.hpp"
#include <chrono>

#define SECURE

#if defined(_DEBUG) // debug mode
    #define LF
    #define XOR(s) (s)
#elif defined(SECURE) // secure mode will hit release only
    #define LF LI_FN
    #define XOR(s) xorstr_(s)
#else // release mode
    #define LF LI_FN
    #define XOR(s) (s)
#endif

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define LOG(s, ...) console::log(XOR(s), __VA_ARGS__)
#define CONSOLE_INFO() console::setColor(CONSOLE_GREEN)
#define CONSOLE_WELCOME() console::setColor(CONSOLE_CYAN)
#define CONSOLE_ERR() console::setColor(CONSOLE_RED)
#define CONSOLE_RESET() console::reset()

namespace utilities
{
	std::string getFolder();

	bool prepareDirectories();

	std::string getHackPath();

    std::string getTime();

	uintptr_t patternScan(const char* mod, const char* mask);
}

inline std::string __DOCUMENTS = utilities::getFolder();

inline std::string __PATH = utilities::getHackPath();

namespace timer
{
	class TimeCount
	{
		using system_clock = std::chrono::system_clock;
		using microsecond = std::chrono::microseconds;
    public:
        TimeCount() = default;
        void start()
        {
            m_Start = system_clock::now();
        }

        void end()
        {
            m_End = system_clock::now();
        }

        double getSec() const
        {
            double timePassed = elapsed().count();
            timePassed *= 1e-6;
            return timePassed;
        }

        std::chrono::system_clock::time_point getStart()
        {
            return m_Start;
        }

        std::chrono::system_clock::time_point getEnd()
        {
            return m_End;
        }
    private:
        system_clock::time_point m_Start;
        system_clock::time_point m_End;

        microsecond elapsed() const
        {
            return std::chrono::duration_cast<microsecond>(m_End - m_Start);
        }
    };
}

inline timer::TimeCount __time;