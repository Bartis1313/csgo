#pragma once

#include <magic_enum.hpp>

// Returns a copy of enum_names() as std::array<std::string, SIZE> with "pretty name"
   // pretty name means:
   // MY_NAME -> My name
   // OKAYLONG -> Okaylong
namespace magic_enum {
    template <typename E>
    [[nodiscard]] constexpr auto enum_names_pretty() noexcept {
        constexpr auto ctoLower = [](char c) noexcept {
            return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
        };

        constexpr auto view_arr = detail::names_v<std::decay_t<E>>;
        std::array<std::string, view_arr.size()> pretty;

        for (size_t i = 0; i < view_arr.size(); i++) {
            std::string name{ view_arr[i].data(), view_arr[i].length() };
            for (size_t sign = 1; sign < name.length(); sign++) {
                name[sign] = ctoLower(name[sign]);

                if (name[sign] == '_')
                    name[sign] = ' ';
            }
            pretty[i] = name;
        }

        return pretty;
    }
}