#pragma once

#include <string>
#include <vector>
#include <utility>
#include <format>
#include "../dependencies/lazy_importer.hpp"
#include "../dependencies/xorstr.hpp"

class Entity_t;
struct Box;
struct Box3D;
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

// enum to type
template<typename T>
_NODISCARD inline constexpr auto E2T(T en) { return static_cast<std::underlying_type_t<T>>(en); }

_NODISCARD inline constexpr std::string operator"" _u8str(const char8_t* str, size_t s)
{
    return std::string{ str, str + s };
}

// format string at runtime
template<typename... Args_t>
_NODISCARD inline constexpr std::string FORMAT(const std::string_view fmt, Args_t&&... args)
{
    return std::vformat(std::locale(), fmt, std::make_format_args(args...));
}

namespace utilities
{
    _NODISCARD std::string getTime();
    // old style: "A0 15 ?? FF A3"
    _NODISCARD uintptr_t patternScan(const std::string& mod, const std::string& mask, const uintptr_t offsetToAdd = 0);
    // https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bounding_ESP_Boxes
    // 2d & 3d
    _NODISCARD bool getBox(Entity_t* ent, Box& box, Box3D& box3D);
    // 2d
    _NODISCARD bool getBox(Entity_t* ent, Box& box);
    _NODISCARD uint32_t inByteOrder(const size_t netLong);
    _NODISCARD std::string getKeyName(const uint32_t virtualKey);
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
    [[deprecated("use key classes")]]
    uint32_t getKey(const uint32_t vKey);
    _NODISCARD float scaleDamageArmor(float dmg, const float armor);
    _NODISCARD std::string u8toStr(const std::u8string& u8str);
    _NODISCARD uintptr_t* findHudElement(const std::string_view name);
    // local
    _NODISCARD bool isChatOpen();
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
    _NODISCARD static std::enable_if_t<__randomHelper::is_integral_v<T>, T> getRandom(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(rng);
    }
    template<typename T>
    _NODISCARD static std::enable_if_t<__randomHelper::is_floating_v<T>, T> getRandom(T min, T max)
    {
        return std::uniform_real_distribution<T>{min, max}(rng);
    }
private:
    inline static std::mt19937 rng{ std::random_device{}() };
};
