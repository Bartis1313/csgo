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

#define LI_EXPORT(var, type, name, _module) \
	void(*name); \
	var = type(LI_FN(name).in(_module));

#define LI_EXPORT_CDECL(var, type, name, _module) \
	void(*name); \
	var = type(LI_FN(name).in(_module))();