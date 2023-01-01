#include "hooks.hpp"

#include "../classes/screen2dEffects.hpp"

hooks::screen2DEffect::value FASTCALL hooks::screen2DEffect::hooked(FAST_ARGS, CViewSetup* view)
{
	Screen2DEffectsType::runAll();

	original(thisptr, view);
}