#include "x88menu.hpp"

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <SDK/structs/Entity.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/inputSystem.hpp>
#include <magic_enum.hpp>


void X88Menu::draw()
{
	if (!vars::keys->enabledX88Menu)
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
		surfaceRender.text(x, y, font, "Hello undefined :)", false, Colors::Yellow);
		// going to hardcode the padding for this
		y += 15;
	}
	else
	{
		surfaceRender.text(x, y, font,
			std::format("Hello {} :) Local Player {:#0x}", game::localPlayer->getName(), game::localPlayer->getLiteralAddress()),
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
		const auto& name = x88p.first;

		auto vecSize = surfaceRender.getTextSizeXY(font, name);
		auto vecX = static_cast<int>(vecSize[Coord::X]);
		auto vecY = static_cast<int>(vecSize[Coord::Y]);

		if (std::holds_alternative<bool*>(value))
		{
			auto val = *std::get<bool*>(value);
			Color active = val ? Colors::LightBlue : Colors::White;

			surfaceRender.text(x, y, font, name, false, color);
			surfaceRender.text(x + addSpaces(name) + vecX, y, font,
				std::format("{}", val), false, active);
			y += vecY;
		}
		else if(std::holds_alternative<int*>(value))
		{
			surfaceRender.text(x, y, font, name, false, color);
			surfaceRender.text(x + addSpaces(name) + vecX, y, font,
				std::format("{}", *std::get<int*>(value)), false, color);
			y += vecY;
		}
		else if (std::holds_alternative<float*>(value))
		{
			surfaceRender.text(x, y, font, name, false, color);
			surfaceRender.text(x + addSpaces(name) + vecX, y, font,
				std::format("{:.2f}", *std::get<float*>(value)), false, color);
			y += vecY;
		}

		i++;
	}

}

void X88Menu::init()
{
	x88types.push("Chams", &vars::visuals->chams->indexPlayers, magic_enum::enum_count<ChamsType>() - 1);
	x88types.push("ESP", &vars::visuals->esp->boxes->enabled);
	x88types.push("FOV", &vars::misc->fov->value, { -50.0f, 50.0f} );
	x88types.push("Backtrack", &vars::backtrack->enabled);
	x88types.push("Backtrack MS", &vars::backtrack->time, { 0.0f, 200.0f });
	x88types.push("No Sky", &vars::visuals->world->sky->removeSky);
	x88types.push("2D Radar", &vars::misc->radar->enabled);
	x88types.push("Bunnyhop", &vars::misc->bunnyHop->enabled);
	x88types.push("Autostrafe", &vars::misc->bunnyHop->indexStrafe, magic_enum::enum_count<MovementStraferMode>() - 1);
	x88types.push("ThirdP", &vars::misc->thirdp->enabled);
	x88types.push("Draw Info", &vars::misc->info->enabled);

	size_t longest = 0;
	for (const auto& [x88p, limits] : x88types.getVars())
	{
		if (auto size = static_cast<size_t>(surfaceRender.getTextSizeXY(fonts::tahoma, x88p.first)[Coord::X]); size > longest)
			longest = size;
	}
	m_longestNameSize = longest;

	m_inited = true;
}

size_t X88Menu::addSpaces(const std::string& text)
{
	// 5px added to align them well for max size
	auto size = (m_longestNameSize + 5) - surfaceRender.getTextSizeXY(fonts::tahoma, text)[Coord::X];
	return static_cast<size_t>(size);
}

void X88Menu::updateKeys()
{
	if (!vars::keys->enabledX88Menu)
		return;

	if (!m_inited)
		return;

	if (KeysHandler::isKeyPressed(VK_DOWN))
	{
		if (index != x88types.getVars().size() - 1)
			index++;
		else
			index = 0;
	}

	if (KeysHandler::isKeyPressed(VK_UP))
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
		if (KeysHandler::isKeyPressed(VK_RIGHT) || KeysHandler::isKeyPressed(VK_LEFT))
		{
			*std::get<bool*>(value) = !*std::get<bool*>(value);
		}
	}
	else if (std::holds_alternative<int*>(value)) // no passed limits in negative, since I throw only indexes
	{
		// run on stack to make it safe for values changed by
		auto valNow = *std::get<int*>(value);

		if (KeysHandler::isKeyPressed(VK_RIGHT))
		{
			valNow++;
			if (valNow > limits.m_intLimtis)
				valNow = 0;

			*std::get<int*>(value) = valNow;
		}
		if (KeysHandler::isKeyPressed(VK_LEFT))
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

		if (KeysHandler::isKeyDown(VK_RIGHT))
		{
			valNow += 0.5f;
			if (valNow > limits.m_floatLimits.second) // max
				valNow = limits.m_floatLimits.first; // min

			*std::get<float*>(value) = valNow;
		}
		if (KeysHandler::isKeyDown(VK_LEFT))
		{
			valNow -= 0.5f;
			if (valNow < limits.m_floatLimits.first)
				valNow = limits.m_floatLimits.second;

			*std::get<float*>(value) = valNow;
		}
	}
}