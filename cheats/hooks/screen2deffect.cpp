#include "hooks.hpp"

#include "../features/visuals/world.hpp"

void __stdcall hooks::screen2DEffect::hooked(CViewSetup* view)
{
	world.drawEffects();

	original(view);
}