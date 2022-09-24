#pragma once

#include <type_traits>
#include <string>
#include <locale>
#include <format>

template<typename T>
[[nodiscard]] inline constexpr auto E2T(T en)
{ 
    return static_cast<std::underlying_type_t<T>>(en);
}

[[nodiscard]] inline constexpr std::string operator"" _u8str(const char8_t* str, size_t s)
{
    return std::string{ str, str + s };
}

// format string at runtime
template<typename... Args_t>
[[nodiscard]] inline constexpr std::string FORMAT(const std::string_view fmt, Args_t&&... args)
{
    return std::vformat(std::locale(), fmt, std::make_format_args(args...));
}
