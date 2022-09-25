#include "hooks.hpp"

#include "../classes/screen2dEffects.hpp"

void __stdcall hooks::screen2DEffect::hooked(CViewSetup* view)
{
	Screen2DEffectsType::runAll();

	original(view);
}