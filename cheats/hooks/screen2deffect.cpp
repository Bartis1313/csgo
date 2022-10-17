#include "hooks.hpp"

#include "../classes/screen2dEffects.hpp"

void __fastcall hooks::screen2DEffect::hooked(FAST_ARGS, CViewSetup* view)
{
	Screen2DEffectsType::runAll();

	original(thisptr, view);
}