#include "x88menu.hpp"

#include <game/game.hpp>
#include <game/globals.hpp>
#include <SDK/structs/Entity.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/inputSystem.hpp>

#include <format>

void X88Menu::draw()
{
	Vector2D man;

	if (!config.get<bool>(vars.bMenuOpenedx88))
		return;

	if (!m_inited)
		return;

	const static Color highlight = Colors::Grey.getColorEditAlpha(0.85f);
	const static Color normal = Colors::White;
	const static auto font = fonts::tahoma;

	int x = static_cast<int>(globals::screenX * 0.2f);
	int y = 20;

	// start
	if (!game::localPlayer)
	{
		surfaceRender.text(x, y, font, XOR("Hello undefined :)"), false, Colors::Yellow);
		// going to hardcode the padding for this
		y += 15;
	}
	else
	{
		surfaceRender.text(x, y, font,
			FORMAT(XOR("Hello {} :) Local Player {:#0x}"), game::localPlayer->getName(), game::localPlayer->getLiteralAddress()),
			false, Colors::Yellow);
		y += 15;
	}

	int yBackup = y;
	
	for (size_t i = 0; const auto& [x88p, limits] : x88types.getVars())
	{
		Color color = index == i ? highlight : normal;

		if (!(i % 10) && i != 0)
		{
			x += 150; // hardcode
			y = yBackup;
		}

		auto value = x88p.second;
		auto name = x88p.first;

		auto vecSize = surfaceRender.getTextSizeXY(font, name);
		auto vecX = static_cast<int>(vecSize.x);
		auto vecY = static_cast<int>(vecSize.y);

		if (std::holds_alternative<bool*>(value))
		{
			auto val = *std::get<bool*>(value);
			Color active = val ? Colors::LightBlue : Colors::White;

			surfaceRender.text(x, y, font, name, false, color);
			surfaceRender.text(x + addSpaces(name) + vecX, y, font,
				FORMAT(XOR("{}"), val), false, active);
			y += vecY;
		}
		else if(std::holds_alternative<int*>(value))
		{
			surfaceRender.text(x, y, font, name, false, color);
			surfaceRender.text(x + addSpaces(name) + vecX, y, font,
				FORMAT(XOR("{}"), *std::get<int*>(value)), false, color);
			y += vecY;
		}
		else if (std::holds_alternative<float*>(value))
		{
			surfaceRender.text(x, y, font, name, false, color);
			surfaceRender.text(x + addSpaces(name) + vecX, y, font,
				FORMAT(XOR("{:.2f}"), *std::get<float*>(value)), false, color);
			y += vecY;
		}

		i++;
	}

}

void X88Menu::init()
{
	x88types.push(XOR("Chams"), &config.getRef<int>(vars.iChamsPlayers), 4);
	x88types.push(XOR("ESP"), &config.getRef<bool>(vars.bEsp));
	x88types.push(XOR("FOV"), &config.getRef<float>(vars.fFOV), { -50.0f, 50.0f} );
	x88types.push(XOR("Backtrack"), &config.getRef<bool>(vars.bBacktrack));
	x88types.push(XOR("Backtrack MS"), &config.getRef<float>(vars.fBacktrackTick), { 0.0f, 200.0f });
	x88types.push(XOR("No Sky"), &config.getRef<bool>(vars.bRemoveSky));
	x88types.push(XOR("2D Radar"), &config.getRef<bool>(vars.bRadar));
	x88types.push(XOR("Bunnyhop"), &config.getRef<bool>(vars.bBunnyHop));
	x88types.push(XOR("Autostrafe"), &config.getRef<int>(vars.iAutoStrafe), 3);
	x88types.push(XOR("ThirdP"), &config.getRef<bool>(vars.bThirdp));
	x88types.push(XOR("Draw Info"), &config.getRef<bool>(vars.bDrawMiscInfo));

	size_t longest = 0;
	for (const auto& [x88p, limits] : x88types.getVars())
	{
		if (auto size = static_cast<size_t>(surfaceRender.getTextSizeXY(fonts::tahoma, x88p.first).x); size > longest)
			longest = size;
	}
	m_longestNameSize = longest;

	m_inited = true;
}

size_t X88Menu::addSpaces(const std::string& text)
{
	// 5px added to align them well for max size
	auto size = (m_longestNameSize + 5) - surfaceRender.getTextSizeXY(fonts::tahoma, text).x;
	return static_cast<size_t>(size);
}

void X88Menu::handleKeys()
{
	if (!config.get<bool>(vars.bMenuOpenedx88))
		return;

	if (!m_inited)
		return;

	if (inputHandler.isKeyPressed(VK_DOWN))
	{
		if (index != x88types.getVars().size() - 1)
			index++;
		else
			index = 0;
	}

	if (inputHandler.isKeyPressed(VK_UP))
	{
		if (index != 0)
			index--;
		else
			index = x88types.getVars().size() - 1;
	}

	auto current = x88types.getVars().at(index);
	auto& value = current.first.second;
	auto limits = current.second;

	if (std::holds_alternative<bool*>(value))
	{
		// don't care, bools have 0 or 1
		if (inputHandler.isKeyPressed(VK_RIGHT) || inputHandler.isKeyPressed(VK_LEFT))
		{
			*std::get<bool*>(value) = !*std::get<bool*>(value);
		}
	}
	else if (std::holds_alternative<int*>(value)) // no passed limits in negative, since I throw only indexes
	{
		// run on stack to make it safe for values changed by
		auto valNow = *std::get<int*>(value);

		if (inputHandler.isKeyPressed(VK_RIGHT))
		{
			valNow++;
			if (valNow > limits.m_intLimtis)
				valNow = 0;

			*std::get<int*>(value) = valNow;
		}
		if (inputHandler.isKeyPressed(VK_LEFT))
		{
			valNow--;
			if (valNow < 0)
				valNow = limits.m_intLimtis;

			*std::get<int*>(value) = valNow;
		}
	}
	else if (std::holds_alternative<float*>(value)) // passed both limits
	{
		auto valNow = *std::get<float*>(value);

		if (inputHandler.isKeyDown(VK_RIGHT))
		{
			valNow += 0.5f;
			if (valNow > limits.m_floatLimits.second) // max
				valNow = limits.m_floatLimits.first; // min

			*std::get<float*>(value) = valNow;
		}
		if (inputHandler.isKeyDown(VK_LEFT))
		{
			valNow -= 0.5f;
			if (valNow < limits.m_floatLimits.first)
				valNow = limits.m_floatLimits.second;

			*std::get<float*>(value) = valNow;
		}
	}
}