#pragma once

#include <lazy_importer.hpp>

#include <string_view>
#include <initializer_list>

#define LI_FN_CACHED(name) \
    LI_FN(name).cached()

namespace spoof
{
    uintptr_t getGadget(const std::string_view moduleName, const std::initializer_list<uint8_t>& jmpRbxBytes = { 0xFF, 0x23 });
}