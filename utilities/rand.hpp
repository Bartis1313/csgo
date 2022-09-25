#pragma once

#include <random>
#include <type_traits>

namespace __randomHelper // helper to know when given type is a real int, not like casted chars
{
    template<typename T>
    inline bool constexpr is_character_v
    {
        std::is_same_v<T, char> ||
        std::is_same_v<T, wchar_t> ||
        std::is_same_v<T, char16_t> ||
        std::is_same_v<T, char32_t> ||
        std::is_same_v<T, char8_t>
    };
    template<typename T>
    inline bool constexpr is_integral_v{ std::is_integral_v<T> && !is_character_v<T> };
    template<typename T>
    inline bool constexpr is_floating_v{ std::is_floating_point_v<T> };
}

class Random // sfinae rule simple template static class
{
public:
    template<typename T>
    [[nodiscard]] static std::enable_if_t<__randomHelper::is_integral_v<T>, T> getRandom(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(rng);
    }
    template<typename T>
    [[nodiscard]] static std::enable_if_t<__randomHelper::is_floating_v<T>, T> getRandom(T min, T max)
    {
        return std::uniform_real_distribution<T>{min, max}(rng);
    }
private:
    inline static std::mt19937 rng{ std::random_device{}() };
};