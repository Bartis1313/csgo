#pragma once

#include <windef.h>
#include <initializer_list>

namespace inputSystem
{
    void run(UINT message, WPARAM wparam);
    [[nodiscard]] bool isKeyDown(UINT vKey);
    [[nodiscard]] bool isKeyPressed(UINT vKey);
    [[nodiscard]] bool isKeyPressed(const std::initializer_list<UINT>& vKeys);
    [[nodiscard]] bool isKeyDown(const std::initializer_list<UINT>& vKeys);
};