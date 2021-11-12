#pragma once
#include "../dependencies/lazy_importer.hpp"
#include "../dependencies/xorstr.hpp"
#include "console/console.hpp"
#include <string>

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

inline void LOG(const short type, const std::string& str) { console::log(type, std::cref(str)); }

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