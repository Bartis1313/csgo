#pragma once
#include "../dependencies/lazy_importer.hpp"
#include "../dependencies/xorstr.hpp"
#include "console/console.hpp"
#include <string>
#include <span>
#include <vector>
#include <algorithm>

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

inline void LOG(const short type, const std::string& str) { console::log(type, str); }

namespace utilities
{
    std::string getFolder();
    bool prepareDirectories();
    std::string getHackPath();
    std::string getTime();
    // old style: "A0 15 ?? FF A3"
    uintptr_t patternScan(const char* mod, const char* mask);
    // TODO: make it in verctor class later
    inline float distToMeters(const float dist) { return dist * 0.0254f; }
    // https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bounding_ESP_Boxes
    bool getBox(Entity_t* ent, Box& box);
    size_t inByteOrder(const size_t netLong);
    std::string getKeyName(UINT virtualKey);
    std::string toLowerCase(const std::string& str);
    // return filled array
    template<typename T, size_t howMany>
    std::span<T> getFilledArray(const T& toFill)
    {
        std::span<T> toReturn(howMany);
        std::fill(toReturn.begin(), toReturn.begin() + howMany, toFill);
        return toReturn;
    }
    // make argument as filled
    template<typename T>
    void fillArray(std::span<T>& arr, const T& toFill)
    {
        std::fill(arr.begin(), arr.end(), toFill);
    }
    std::vector<std::string> splitStr(const std::string& str, char limit = ' ');
}

inline std::string __DOCUMENTS = utilities::getFolder();
inline std::string __PATH = utilities::getHackPath();