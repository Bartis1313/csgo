#pragma once

#include <string>
#include <vector>

class Entity_t;
struct Box;
struct Vector;

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
    [[nodiscard]] float scaleDamageArmor(float dmg, const float armor);
    [[nodiscard]] std::string u8toStr(const std::u8string& u8str);
    [[nodiscard]] uintptr_t* findHudElement(const std::string_view name);
    // local
    [[nodiscard]] bool isChatOpen();
    [[nodiscard]] float getScaledFont(const Vector& source, const Vector& destination, const float division = 80.0f, const float min = 12.0f, const float max = 30.0f);
}

#include <random>
#include <type_traits>

namespace __randomHelper // helper to know when given type is a real int, not like casted chars
{
    template<typename T>
    inline bool constexpr is_character_v
    {
        std::is_same_v<T, char>     ||
        std::is_same_v<T, wchar_t>  ||
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
