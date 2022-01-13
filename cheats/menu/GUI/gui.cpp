#include "gui.hpp"
#include "TextureHolder.hpp"
#include "../../../resource.h"
#include "../../../utilities/utilities.hpp"

void test::init()
{
	fatchad = TextureHolder(IMAGE_PNG1);
	LOG(LOG_INFO, XOR("all images done"));
}

void test::run()
{
	fatchad.draw(400, 500);
}

bool GUI::isKeyDown(const short key)
{
	return globals::keyState[key];
}

bool GUI::isKeyPressed(const short key)
{
	return globals::keyState[key] && !globals::previousKeyState[key];
}

void GUI::drawspectre()
{
	render::drawGradient(300, 500, 40, 80, Colors::Red, Colors::Pink, true);
	render::drawGradient(330, 500, 40, 80, Colors::Pink, Colors::DarkBlue, true);
	render::drawGradient(360, 500, 40, 80, Colors::DarkBlue, Colors::Green, true);
	render::drawGradient(390, 500, 40, 80, Colors::Green, Colors::Yellow, true);
	render::drawGradient(420, 500, 40, 80, Colors::Yellow, Colors::Red, true);
}