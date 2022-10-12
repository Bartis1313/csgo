#pragma once

#include <dependencies/lazy_importer.hpp>
#include <dependencies/xorstr.hpp>

#if _DEBUG
#define XOR(s) (s)
#else
#define XOR(s) xorstr_(s)
#endif

#define LI_FN_CACHED(name) \
    LI_FN(name).cached()