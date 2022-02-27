#pragma once
#include "Color.hpp"
#include "../utilities/vfunc.hpp"

class ConCommand;
class IConVar;

class ICvar
{
public:
    VFUNC(IConVar*, findVar, 15, (const char* name), (this, name));
    VFUNC(ConCommand*, findCommand, 17, (const char* name), (this, name));
    template <typename... Args_t>
    void consoleColorPrintf(const Color& clr, const char* fmt, Args_t... args)
    {
        return vfunc::callVPack<void, 25, const Color&, const char*, Args_t...>(this, std::cref(clr), fmt, args...);
    }
    template<typename... Args_t>
    void consolePrintf(const char* fmt, Args_t... args)
    {
        return vfunc::callVPack<void, 26, const char*, Args_t...>(this, fmt, args...);
    }
};