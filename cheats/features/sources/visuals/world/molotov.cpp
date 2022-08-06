#include "molotov.hpp"

#include "../../../../../SDK/CGlobalVars.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"
#include "../../../../../utilities/math/math.hpp"

void MolotovDraw::draw(Inferno_t* ent)
{
	if (!config.get<bool>(vars.bDrawmolotovRange))
		return;

	auto molotov = reinterpret_cast<Inferno_t*>(ent);
	if (!molotov)
		return;

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/server/cstrike15/Effects/inferno.cpp
	// here you can see ratios and everything

	//Vector min = {}, max = {};
	//ent->getRenderBounds(min, max);
	//imRender.drawCircle3DFilled(molotov->absOrigin(), 0.5f * Vector(max - min).length2D(), 32, config.get<Color>(vars.cMolotovRange));

	//imRender.text(screen.x, screen.y, ImFonts::tahoma, XOR("Molotov"), false, config.get<Color>(vars.cMolotovRangeText));
	float time = molotov->spawnTime() + molotov->expireTime() - interfaces::globalVars->m_curtime;
	float scale = time / molotov->expireTime();

	const auto& origin = molotov->absOrigin();
	constexpr int molotovRadius = 60; // 30 * 2

	//std::vector<ImVec2> points = {};
	CfgColor col = config.get<CfgColor>(vars.cMolotovRange);

	//std::vector<ImVec2> points;

	for (int i = 0; i < molotov->m_fireCount(); i++)
	{
		auto pos = origin + molotov->getInfernoPos(i);
		imRender.drawCircle3DFilled(pos, molotovRadius, 32, col.getColor(), col.getColor());

		/*ImVec2 posw;
		if (!imRender.worldToScreen(pos, posw))
			break;

		points.push_back(posw);*/

		//imRender.text(posw.x, posw.y, ImFonts::tahoma, std::to_string(i), false, Colors::Cyan);*/
		// points are not on edge, this will need some graph path logic, and will be done soon
	}
	static float size = ImFonts::tahoma14->FontSize;
	// timer
	if (ImVec2 s; imRender.worldToScreen(origin, s))
	{
		imRender.drawProgressRing(s.x, s.y, 25, 32, -90.0f, scale, 5.0f, Colors::LightBlue);
		imRender.text(s.x, s.y - (size / 2.0f), ImFonts::tahoma14, FORMAT(XOR("{:.2f}s"), time), true, Colors::White);
	}

	/*auto g = math::giftWrap(points);
	imRender.drawPolyGon(g, Colors::LightBlue);*/ // still too less points to make is "cool"
}