#pragma once

#include <string_view>
#include <initializer_list>

namespace spoof
{
    uintptr_t getGadget(const std::string_view moduleName, const std::initializer_list<uint8_t>& jmpRbxBytes = { 0xFF, 0x23 });
}