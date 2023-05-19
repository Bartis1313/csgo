#pragma once

#include <array>

// is not a current feature in std
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

    constexpr const char* c_str() const 
    {
        return this->data();
    }
};

template<size_t SIZE>
cStr(const char(&)[SIZE]) -> cStr<SIZE - 1U>;