#pragma once

#include "../hooks.hpp"

#include <utilities/runnable.hpp>
#include <cheats/game/globals.hpp>

#define _HOOK_BEGIN			namespace hooks {
#define _HOOK_END			}

// we somehow need to pass option for private fields for place where hooks init
#define ADD_HOOK_FRIEND		friend void hooks::init(); friend void hooks::shutdown();