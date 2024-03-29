#include "x88menu.hpp"

#include "x88types.hpp"

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <SDK/structs/Entity.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/inputSystem.hpp>
#include <magic_enum.hpp>

#include <cheats/hooks/wndproc.hpp>

#define ORIGINAL_DRAW true
#if ORIGINAL_DRAW == true
#include <cheats/hooks/paintTraverse.hpp>
#else
#include <cheats/hooks/present.hpp>
#endif

namespace
{
#if ORIGINAL_DRAW == true
	struct x88Draw : hooks::PaintTraverse
	{
		x88Draw()
		{
			this->registerRender(x88Menu::draw);
		}
	} x88Draw;
#else
	struct x88Draw : hooks::Present
	{
		x88Draw()
		{
			this->registerRun(x88Menu::draw);
		}
	} x88Draw;
#endif

	struct x88Keys : hooks::wndProcSys
	{
		x88Keys()
		{
			this->registerRun(x88Menu::updateKeys);
		}
	} x88Keys;
}
#if ORIGINAL_DRAW == true
static void drawText(float x, float y, unsigned long font, const std::string& text, const Color& color)
{
	SurfaceRender::text(static_cast<int>(x), static_cast<int>(y), font, text, false, color);
}
#else
static void drawText(float x, float y, ImFont* font, const std::string& text, const Color& color)
{
	drawing::Text{ font, ImVec2{ x, y }, Color::U32(color), text, false, false }.draw(ImGui::GetBackgroundDrawList());
}
#endif

namespace x88Menu
{
	// in pixels, padding for X
	size_t addSpaces(const std::string& text);

	size_t index{ 0 };
	size_t m_longestNameSize{ };
	bool m_inited{ false };
	X88Types x88types{ };
}

void x88Menu::draw()
{
	if (!vars::keys->enabledX88Menu)
		return;

	if (!m_inited)
		return;

	const static Color highlight = Colors::Grey.getColorEditAlpha(0.85f);
	const static Color normal = Colors::White;
#if ORIGINAL_DRAW == true
	const static auto font = SurfaceRender::fonts::tahoma;
#else
	const static auto font = ImRender::fonts::csgoTahoma15;
#endif

	float x = globals::screenX * 0.2f;
	float y = 20.0f;

	if (!game::localPlayer)
	{
		drawText(x, y, font, "Hello undefined :)", Colors::Yellow);
#if ORIGINAL_DRAW == true
		y += 14.0f; // size of tahoma
#else
		y += font->FontSize;
#endif
	}
	else
	{
		drawText(x, y, font, std::format("Hello {} :) Local Player {:#0x}", game::localPlayer->getName(), game::localPlayer->getLiteralAddress()), Colors::Yellow);
#if ORIGINAL_DRAW == true
		y += 14.0f; // size of tahoma
#else
		y += font->FontSize;
#endif
	}

	const float yBackup = y;

	for (size_t i = 0; const auto& [x88p, limits] : x88types.getVars())
	{
		Color color = normal;
		if (vars::keys->x88Toggle.isToggled())
		{
			if (index == i)
				color = highlight;
		}

		if (!(i % 10) && i != 0)
		{
			x += 150.0f; // hardcode
			y = yBackup;
		}

		const auto value = x88p.second;
		const auto& name = x88p.first;
#if ORIGINAL_DRAW == true
		const auto __vSize = SurfaceRender::getTextSizeXY(font, name);
		const auto vecSize = ImVec2{ __vSize[0], __vSize[1] };
#else
		const auto vecSize = ImRender::getTextSize(font, font->FontSize, name);
#endif

		if (std::holds_alternative<bool*>(value))
		{
			auto val = *std::get<bool*>(value);
			Color active = val ? Colors::LightBlue : Colors::White;

			drawText(x, y, font, name, color);
			drawText(x + addSpaces(name) + vecSize.x, y, font, std::format("{}", val), active);
			y += vecSize.y;
		}
		else if(std::holds_alternative<int*>(value))
		{
			drawText(x, y, font, name, color);
			drawText(x + addSpaces(name) + vecSize.x, y, font, std::format("{}", *std::get<int*>(value)), color);
			y += vecSize.y;
		}
		else if (std::holds_alternative<float*>(value))
		{
			drawText(x, y, font, name, color);
			drawText(x + addSpaces(name) + vecSize.x, y, font, std::format("{:.2f}", *std::get<float*>(value)), color);
			y += vecSize.y;
		}

		i++;
	}

}

void x88Menu::setStyles()
{
	//x88types.push("Chams", &vars::visuals->chams->indexPlayers, magic_enum::enum_count<ChamsType>() - 1);
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
#if ORIGINAL_DRAW == true
	for (const auto& [x88p, limits] : x88types.getVars())
	{
		if (auto size = static_cast<size_t>(SurfaceRender::getTextSize(SurfaceRender::fonts::tahoma, x88p.first)); size > longest)
			longest = size;
	}
#else
	for (const auto& [x88p, limits] : x88types.getVars())
	{
		if (auto size = static_cast<size_t>(ImRender::getTextSize(ImRender::fonts::csgoTahoma15, ImRender::fonts::csgoTahoma15->FontSize, x88p.first).x); size > longest)
			longest = size;
	}
#endif
	m_longestNameSize = longest;

	m_inited = true;
}

size_t x88Menu::addSpaces(const std::string& text)
{
	// 5px added to align them well for max size
#if ORIGINAL_DRAW == true
	auto size = (m_longestNameSize + 5) - SurfaceRender::getTextSize(SurfaceRender::fonts::tahoma, text);
	return static_cast<size_t>(size);
#else
	auto size = (m_longestNameSize + 5) - ImRender::getTextSize(ImRender::fonts::csgoTahoma15, ImRender::fonts::csgoTahoma15->FontSize, text).x;
	return static_cast<size_t>(size);
#endif
}

void x88Menu::updateKeys()
{
	if (!vars::keys->enabledX88Menu)
		return;

	vars::keys->x88Toggle.update();

	if (!vars::keys->x88Toggle.isToggled())
		return;

	if (!m_inited)
		return;

	if (inputSystem::isKeyPressed(VK_DOWN))
	{
		if (index != x88types.getVars().size() - 1)
			index++;
		else
			index = 0;
	}

	if (inputSystem::isKeyPressed(VK_UP))
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
		if (inputSystem::isKeyPressed(VK_RIGHT) || inputSystem::isKeyPressed(VK_LEFT))
		{
			*std::get<bool*>(value) = !*std::get<bool*>(value);
		}
	}
	else if (std::holds_alternative<int*>(value)) // no passed limits in negative, since I throw only indexes
	{
		// run on stack to make it safe for values changed by
		auto valNow = *std::get<int*>(value);

		if (inputSystem::isKeyPressed(VK_RIGHT))
		{
			valNow++;
			if (valNow > limits.m_intLimtis)
				valNow = 0;

			*std::get<int*>(value) = valNow;
		}
		if (inputSystem::isKeyPressed(VK_LEFT))
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

		if (inputSystem::isKeyDown(VK_RIGHT))
		{
			valNow += 0.5f;
			if (valNow > limits.m_floatLimits.second) // max
				valNow = limits.m_floatLimits.first; // min

			*std::get<float*>(value) = valNow;
		}
		if (inputSystem::isKeyDown(VK_LEFT))
		{
			valNow -= 0.5f;
			if (valNow < limits.m_floatLimits.first)
				valNow = limits.m_floatLimits.second;

			*std::get<float*>(value) = valNow;
		}
	}
}