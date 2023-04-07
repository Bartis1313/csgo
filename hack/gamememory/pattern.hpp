#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace signature
{
    constexpr char spaceSign{ ' ' };
    constexpr char wildcard{ '?' };

    namespace helpers
    {
        template <typename T>
        constexpr bool isHexDigit(T c)
        {
            return
                (c >= '0' && c <= '9') ||
                (c >= 'a' && c <= 'f') ||
                (c >= 'A' && c <= 'F');
        }

        template <typename T>
        constexpr T hexToDigit(T c)
        {
            if (c >= '0' && c <= '9')
                return c - '0';
            else if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            else return 0;
        }

        constexpr char digitToHex(uint8_t digit)
        {
            if (digit < 10)
                return static_cast<char>(digit + '0');
            else
                return static_cast<char>(digit - 10 + 'A');
        }
    }

    // is not a curent feature in std
    // https://youtu.be/zRYlQGMdISI?t=145
    template<size_t SIZE>
    struct cStr : public std::array<char, SIZE + 1U>
    {
        constexpr cStr(const char* str)
            : std::array<char, SIZE + 1U>()
        {
            for (size_t i = 0U; i != SIZE; ++i)
                this->_Elems[i] = str[i];
        }
    };

    template<size_t SIZE>
    cStr(const char(&)[SIZE]) -> cStr<SIZE - 1U>;

    template <cStr str>
    struct cSig
    {
    private:
        // won't work for multiwildcards
        constexpr static auto getSize()
        {
            size_t size{ 0U };
            bool wasNumber{ false };
            for (const char sign : str)
            {
                if (helpers::isHexDigit(sign))
                {
                    if (!wasNumber)
                        ++size;

                    wasNumber = true;
                }

                else if (sign == wildcard)
                {
                    ++size;
                    wasNumber = false;
                }
                else if (sign == spaceSign)
                {
                    wasNumber = false;
                }
            }

            return size;
        }
    public:
        constexpr static auto sig()
        {
            std::array<std::optional<uint8_t>, getSize()> result{};
            std::size_t i{ 0U };
            std::size_t j{ 0U };
            while (i < str.size() && j != result.size())
            {
                if (str[i] == wildcard)
                {
                    result[j] = std::nullopt;
                    i++;
                }
                else if (str[i] == spaceSign)
                {
                    i++;
                    continue;
                }
                else if (helpers::isHexDigit(str[i]) && helpers::isHexDigit(str[i + 1]))
                {
                    result[j] = (helpers::hexToDigit(str[i]) << 4) | helpers::hexToDigit(str[i + 1]);
                    i += 2;
                }
                j++;
            }
            return result;
        }
    };

    // only for single wildcards
    template<size_t SIZE>
    constexpr std::string_view to_string(const std::array<std::optional<uint8_t>, SIZE>& s)
    {
        constexpr size_t maxSize{ (SIZE * 3) - 1 };
        std::array<char, maxSize> buffer{ };
        for (size_t i = 0; const auto & sigByte : s)
        {
            if (sigByte.has_value())
            {
                const uint8_t byte{ sigByte.value() };
                buffer[i++] = helpers::digitToHex(byte >> 4);
                buffer[i++] = helpers::digitToHex(byte & 0x0F);
                buffer[i++] = spaceSign;
            }
            else
            {
                buffer[i++] = wildcard;
                buffer[i++] = spaceSign;
            }
        }

        return std::string_view{ buffer.data(), buffer.size() - 1 };
    }
}

#define SIG(_sig) signature::cSig<_sig>::sig()