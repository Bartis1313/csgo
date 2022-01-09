#include "hooks.hpp"
#include "../menu/menu.hpp"
#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../menu/GUI/gui.hpp"

#pragma region "Paint Helpers"
bool shouldReloadsFonts()
{
	static int oldX, oldY, x, y;
	interfaces::engine->getScreenSize(x, y);

	if (x != oldX || y != oldY)
	{
		oldX = x;
		oldY = y;
		return true;
	}

	return false;
}

bool isValidWindow()
{
	// sub window is better, for cs as they recently updated main window name
#ifdef _DEBUG
	if (auto window = FindWindowA("Valve001", NULL); GetForegroundWindow() != window)
		return false;
#else
	if (auto window = LF(FindWindowA).cached()(XOR("Valve001"), NULL); LF(GetForegroundWindow).cached()() != window)
		return false;
#endif
	return true;
}

void guiStates()
{
#ifdef _DEBUG
	for (short i = 0; i < 256; i++)
	{
		GUI::globals::previousKeyState[i] = GUI::globals::keyState[i];
		GUI::globals::keyState[i] = GetAsyncKeyState(i);
	}
#else
	for (short i = 0; i < 256; i++)
	{
		GUI::globals::previousKeyState[i] = GUI::globals::keyState[i];
		GUI::globals::keyState[i] = LF(GetAsyncKeyState).cached()(i);
	}
#endif
	interfaces::surface->getCursor(GUI::globals::mouseX, GUI::globals::mouseY);
}

#pragma endregion

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	if (!isValidWindow())
		return;

	// will run first no matter what, you can edit the function a bit or add ghetto static counter
	if (shouldReloadsFonts())
		render::init();

	if (strstr(interfaces::panel->getName(panel), XOR("HudZoom")))
	{
		if (interfaces::engine->isInGame())
			return;
	}

	original(interfaces::panel, panel, forceRepaint, allowForce);

	if (strstr(interfaces::panel->getName(panel), XOR("MatSystemTopPanel")))
	{
		guiStates();
		//Menu::g().draw();
		//Menu::g().handleKeys();
		esp::run();
		world::drawMisc();
		radar::run();
		misc::drawLocalInfo();
		misc::drawFpsPlot();
		misc::drawVelocityPlot();
		misc::drawHitmarker();
		world::drawZeusRange();
		misc::drawNoScope();
		misc::drawCrosshair();
		// testing image, all good
		//test::run();
		GUI::renderGUI::test();
	}
}