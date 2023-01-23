#pragma once

#include <lazy_importer.hpp>
#include <xorstr.hpp>

#include <string_view>
#include <initializer_list>

#if _DEBUG
#define XOR(s) (s)
#else
#define XOR(s) xorstr_(s)
#endif

#define LI_FN_CACHED(name) \
    LI_FN(name).cached()

#define EXPORT(var, __hash, _module) \
	var = ::li::detail::lazy_function<__hash, decltype(var)>().in(g_Memory.getModule(_module));

namespace spoof
{
    uintptr_t getGadget(const std::string_view moduleName, const std::initializer_list<uint8_t>& jmpRbxBytes = { 0xFF, 0x23 });
}