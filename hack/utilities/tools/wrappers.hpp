#pragma once

#include <lazy_importer.hpp>

#include <type_traits>
#include <string>
#include <format>

template<typename T>
[[nodiscard]] inline constexpr auto E2T(T en)
{ 
    return static_cast<std::underlying_type_t<T>>(en);
}

[[nodiscard]] inline constexpr auto hasher(const std::string_view str)
{
    constexpr auto hashBasis = 2166136261U;
    return li::detail::khash(str.data(), hashBasis);
}

[[nodiscard]] inline constexpr auto operator"" _u8str(const char8_t* str, size_t s)
{
    return std::string{ str, str + s };
}

[[nodiscard]] inline constexpr auto operator"" _hasher(const char* str, size_t s)
{
    return hasher(str);
}
