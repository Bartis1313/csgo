#pragma once

#include <string>
#include <vector>
#include <array>
#include <ranges>

namespace utilities
{
    [[nodiscard]] std::string getTime();
    [[nodiscard]] std::string getKeyName(const uint32_t virtualKey);
    [[nodiscard]] std::string toLowerCase(const std::string& str);
    [[nodiscard]] std::string toUpperCase(const std::string& str);
    template<typename T, size_t SIZE>
    [[nodiscard]] constexpr std::array<T, SIZE> getFilledArray(const T& val)
    {
        std::array<T, SIZE> arr;
        std::ranges::fill(arr, val);
        return arr;
    }
    // deprecated, https://www.youtube.com/watch?v=V14xGZAyVKI
    [[deprecated("use std::views::split")]]
    std::vector<std::string> splitStr(const std::string& str, char limit = ' ');
    [[deprecated("use key classes")]]
    uint32_t getKey(const uint32_t vKey);
    [[nodiscard]] std::string u8toStr(const std::u8string& u8str);
}