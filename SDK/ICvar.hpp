#pragma once
#include <cstdint>
#include "Color.hpp"
#include "../utilities/vfunc.hpp"
#include <type_traits>
#include <array>

class ConCommand;
class IConVar;

class ICvar
{
public:
    VFUNC(IConVar*, FindVar, 15, (const char* name), (this, name));
    VFUNC(ConCommand*, FindCommand, 17, (const char* name), (this, name));
    // may throw exception, but the game does never crash, it's due to va_args used originally by valve
    // to skip the pain I would use simple padding for vfuncs here, last option
    template <typename... args_t>
    void ConsoleColorPrintf(const Color& clr, const char* pFormat, args_t... ca_list)
    {
        return vfunc::callVPack<void, 25, const Color&, const char*, args_t...>(this, std::cref(clr), pFormat, ca_list...);
    }
    template<typename... args_t>
    void ConsolePrintf(const char* pFormat, args_t... ca_list)
    {
        return vfunc::callVPack<void, 26, const char*, args_t...>(this, pFormat, ca_list...);
    }
};