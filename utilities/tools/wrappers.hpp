#pragma once

#include <type_traits>
#include <string>
#include <format>

template<typename T>
[[nodiscard]] inline constexpr auto E2T(T en)
{ 
    return static_cast<std::underlying_type_t<T>>(en);
}

[[nodiscard]] inline constexpr auto hash(const std::string_view str)
{
    return li::detail::khash(str.data());
}

[[nodiscard]] inline constexpr auto operator"" _u8str(const char8_t* str, size_t s)
{
    return std::string{ str, str + s };
}

[[nodiscard]] inline constexpr auto operator"" _hash(const char* str, size_t s)
{
    return hash(str);
}

// format string at runtime
template<typename... Args_t>
[[nodiscard]] inline constexpr std::string FORMAT(const std::string_view fmt, Args_t&&... args)
{
    return std::vformat(fmt, std::make_format_args(args...));
}
