#include "hooks.hpp"
#include "../menu/x88/menuX88.hpp"
#include "../features/visuals/player.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../menu/GUI/drawing.hpp"
#include "../globals.hpp"

#pragma region "Paint Helpers"
bool shouldReloadsFonts()
{
	static int oldX, oldY, x, y;
	x = globals::screenX;
	y = globals::screenY;

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

// run keys
void guiStates()
{
#ifdef _DEBUG
	for (unsigned i = 0; i < 256; i++)
	{
		globals::previousKeyState[i] = globals::keyState[i];
		globals::keyState[i] = static_cast<bool>(GetAsyncKeyState(i));
	}
#else
	for (unsigned i = 0; i < 256; i++)
	{
		globals::previousKeyState[i] = globals::keyState[i];
		globals::keyState[i] = static_cast<bool>(LF(GetAsyncKeyState).cached()(i));
	}
#endif
	interfaces::surface->getCursor(globals::mouseX, globals::mouseY);
}

// run current screensize
void getScreen()
{
	int x, y;
	interfaces::engine->getScreenSize(x, y);
	globals::screenX = x;
	globals::screenY = y;
}

// handle x88 menu keys and drawing
void x88Handle()
{
	x88Menu.handleKeys();
	x88Menu.draw();
}

// draw hack name
void drawWaterMark()
{
	render::text(20, 10, fonts::tahoma, XOR("csgo legit"), false, Colors::Palevioletred);
}

// draw all painting
void paintHandle()
{
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
	legitbot::drawFov();

	config.get<bool>(vars.bMenuOpenedx88)
		? x88Handle()
		: GUI::draw();
}

#pragma endregion

void __stdcall hooks::paintTraverse::hooked(unsigned int panel, bool forceRepaint, bool allowForce)
{
	if (!isValidWindow())
		return;

	getScreen();

	// will run first no matter what, you can hook screensizechanged only for this
	if (shouldReloadsFonts())
		render::init();

	if (interfaces::engine->isTakingScreenshot())
		return;

	static unsigned int panelID = 0;
	static unsigned int panelScope = 0;
	static unsigned int panelFocus = 0;

	const auto panelName = interfaces::panel->getName(panel);

	if (!panelScope)
	{
		if(strstr(panelName, XOR("HudZoom")))
			panelScope = panel;
	}
	else if (panelScope == panel)
	{
		if (interfaces::engine->isInGame() && config.get<bool>(vars.bNoScope))
			return;
	}

	original(interfaces::panel, panel, forceRepaint, allowForce);

	if (!panelID)
	{
		if (strstr(panelName, XOR("MatSystemTopPanel")))
			panelID = panel;
	}
	else if (panelID == panel)
	{
		guiStates();
		paintHandle();
		drawWaterMark();
	}

	if (!panelFocus)
	{
		if (strstr(panelName, XOR("FocusOverlayPanel")))
			panelFocus = panel;
	}
	else if (panelFocus == panel)
	{
		interfaces::panel->setMouseInputEnabled(panel, GUI::menu->isOpened());
	}
}