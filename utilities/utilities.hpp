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
template<typename... args_t>
constexpr inline void LOG(const char* fmt, args_t... ca_list) {
    console::log(fmt, ca_list...);
}
//#define LOG(s, ...) console::log(XOR(s), __VA_ARGS__)
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

    // TODO: make it in verctor class later
    inline float distToMeters(float dist) { return dist * 0.0254f; }
}

inline std::string __DOCUMENTS = utilities::getFolder();

inline std::string __PATH = utilities::getHackPath();