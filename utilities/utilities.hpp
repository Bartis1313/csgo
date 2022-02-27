#pragma once
#include "../dependencies/lazy_importer.hpp"
#include "../dependencies/xorstr.hpp"
#include "console/console.hpp"
#include <string>
#include <vector>

class Entity_t;
struct Box;

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

// logs: into console, into log and into game's console
inline void LOG(const short type, const std::string& str) { console::log(type, str); }

namespace utilities
{
    _NODISCARD std::string getTime();
    // old style: "A0 15 ?? FF A3"
    _NODISCARD uintptr_t patternScan(const std::string& mod, const std::string& mask);
    // https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bounding_ESP_Boxes
    _NODISCARD bool getBox(Entity_t* ent, Box& box);
    _NODISCARD size_t inByteOrder(const size_t netLong);
    _NODISCARD std::string getKeyName(const UINT virtualKey);
    _NODISCARD std::string toLowerCase(const std::string& str);
    _NODISCARD std::string toUpperCase(const std::string& str);
    // return filled array, this should run span<T>, but there were some problems with vector<bool> due to its special magic
    template<typename T, size_t howMany>
    _NODISCARD std::vector<T> getFilledVec(T toFill) // cref not really needed here
    {
        std::vector<T> toReturn(howMany);
        std::fill(toReturn.begin(), toReturn.begin() + howMany, toFill);
        return toReturn;
    } 
    _NODISCARD std::vector<std::string> splitStr(const std::string& str, char limit = ' ');
    _NODISCARD SHORT getKey(const UINT vKey);
}