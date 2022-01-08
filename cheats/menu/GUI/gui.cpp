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