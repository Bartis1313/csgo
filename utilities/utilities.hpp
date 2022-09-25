#pragma once

#include <string>
#include <vector>

namespace utilities
{
    [[nodiscard]] std::string getTime();
    [[nodiscard]] std::string getKeyName(const uint32_t virtualKey);
    [[nodiscard]] std::string toLowerCase(const std::string& str);
    [[nodiscard]] std::string toUpperCase(const std::string& str);
    // return filled array, this should run span<T>, but there were some problems with vector<bool> due to its special magic
    template<typename T, size_t howMany>
    [[nodiscard]] std::vector<T> getFilledVec(T toFill) // cref not really needed here
    {
        std::vector<T> toReturn(howMany);
        std::fill(toReturn.begin(), toReturn.begin() + howMany, toFill);
        return toReturn;
    }
    // deprecated, https://www.youtube.com/watch?v=V14xGZAyVKI
    [[deprecated("use std::views::split")]]
    std::vector<std::string> splitStr(const std::string& str, char limit = ' ');
    [[deprecated("use key classes")]]
    uint32_t getKey(const uint32_t vKey);
    [[nodiscard]] std::string u8toStr(const std::u8string& u8str);
}