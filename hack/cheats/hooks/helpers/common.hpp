#pragma once

#include <vector>
#include <functional>
#include <type_traits>

#include "../hooks.hpp"
#include <cheats/game/globals.hpp>
#include <utilities/runnable.hpp>

#define _HOOK_BEGIN			namespace hooks {
#define _HOOK_END			}

#define FASTCALL			__fastcall
#define THISCALL			__thiscall
#define STDCALL				__stdcall
#define THISPTR				void* thisptr

#define FAST_ARGS			[[maybe_unused]] THISPTR, [[maybe_unused]] void* edx

// we somehow need to pass option for private fields for place where hooks init
#define ADD_HOOK_FRIEND		friend void hooks::init(); friend void hooks::shutdown();