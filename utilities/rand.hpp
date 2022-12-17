#pragma once

#include <random>
#include <type_traits>

#include <random>
#include <type_traits>

class Random // sfinae rule simple template static class
{
public:
    template<typename T>
    [[nodiscard]] static std::enable_if_t<std::is_arithmetic_v<T>, T> getRandom(T min, T max)
    {
        if constexpr (std::is_integral_v<T>)
        {
            return std::uniform_int_distribution<T>{min, max}(rng);
        }
        else
        {
            return std::uniform_real_distribution<T>{min, max}(rng);
        }
    }

private:
    inline static std::mt19937 rng{ std::random_device{}() };
};