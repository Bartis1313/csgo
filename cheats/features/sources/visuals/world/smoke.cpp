#include "smoke.hpp"

#include "../../../../../SDK/CGlobalVars.hpp"
#include "../../../../../SDK/IEffects.hpp"
#include "../../../../../SDK/math/Vector.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/utilities.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"
#include "../../../../../utilities/math/math.hpp"

void SmokeDraw::init()
{

}

void SmokeDraw::draw(Smoke_t* ent)
{
	if (!config.get<bool>(vars.bDrawSmoke))
		return;

	if (!ent->m_nSmokeEffectTickBegin())
	{
		drawCustomSmokeEffect(ent->absOrigin(), 30.0f);
		return;
	}

	float time = game::timeToTicks(ent->m_nSmokeEffectTickBegin()) + ent->expireTime() - interfaces::globalVars->m_curtime;
	float scale = time / ent->expireTime();

	const auto& origin = ent->absOrigin();
	constexpr int smokeRadius = 144;

	//imRender.drawCircle3DFilled(origin, smokeRadius, 216, col, col, true, 2.0f);
	// many points to make it smooth
	drawArc3DSmoke(origin, smokeRadius, 512, scale, config.get<CfgColor>(vars.cDrawSmoke).getColor(), false, 2.0f,
		ImFonts::tahoma14, FORMAT(XOR("{:.2f}s"), time), Colors::Orange);
	drawCustomSmokeEffect(origin, smokeRadius);

	// timer
	/*if (Vector2D s; imRender.worldToScreen(origin, s))
	{
		imRender.drawProgressRing(s.x, s.y, 25, 32, -90, scale, 5.0f, Colors::LightBlue);
		imRender.text(s.x, s.y, ImFonts::tahoma, std::format(XOR("{:.2f}s"), time), false, Colors::White);
	}*/
}

void SmokeDraw::drawArc3DSmoke(const Vector& pos, const float radius, const int points, const float percent,
	const Color& color, const ImDrawFlags flags, const float thickness, ImFont* font, const std::string& text, const Color& colorText)
{
	float step = math::PI *2.0f / points;

	ImVec2 _end = {};
	for (float angle = 0.0f; angle < (math::PI *2.0f * percent); angle += step)
	{
		Vector worldStart = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector worldEnd = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (ImVec2 start, end; imRender.worldToScreen(worldStart, start) && imRender.worldToScreen(worldEnd, end))
		{
			_end = end;
			imRender.drawLine(start, end, color);
		}
	}

	if (_end.x != 0.0f && _end.y != 0.0f)
		imRender.text(_end.x + 3.0f, _end.y, font, text, true, colorText);
}

void SmokeDraw::drawCustomSmokeEffect(const Vector& pos, float radius)
{
	// clockwise for better effect
	Vector end = { radius * std::sin(interfaces::globalVars->m_curtime) + pos.x, radius * std::cos(interfaces::globalVars->m_curtime) + pos.y, pos.z };

	interfaces::effects->smoke(end, -1, 5.0f, 1.0f);
}

void SmokeRemoval::init()
{
	const auto throughSmoke = utilities::patternScan(CLIENT_DLL, GOES_THROUGH_SMOKE);
	m_smokeCountAddr = *reinterpret_cast<uintptr_t*>(throughSmoke + 0x8);
}

void SmokeRemoval::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (!game::localPlayer)
		return;

	if (config.get<bool>(vars.bEditEffectsSmoke)) // remove effects from inside, this is why we nulling smoke count
		*reinterpret_cast<uintptr_t*>(m_smokeCountAddr) = 0;
}
