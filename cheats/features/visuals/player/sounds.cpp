#include "sounds.hpp"

#include "../../events/events.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

void SoundDraw::init()
{
	g_Events.add(XOR("player_footstep"), std::bind(&SoundDraw::pushSteps, this, std::placeholders::_1));
}

void SoundDraw::findBest(Entity_t* ent)
{
	if (!config.get<bool>(vars.bSoundEsp))
		return;

	int index = ent->getIndex();

	float x = globals::screenX / 2.0f;
	float y = globals::screenY / 2.0f;
	float maxDist = config.get<float>(vars.fStepMaxDist);
	float maxDistLine = config.get<float>(vars.fStepMaxLineDist);

	for (size_t i = 0; const auto & el : m_steps.at(index))
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_steps.at(index).erase(m_steps.at(index).begin() + i);
			continue;
		}

		Vector2D elPos;
		if (!imRender.worldToScreen(el.m_pos, elPos))
			continue;

		//float scale = diff / config.get<float>(vars.fStepTime); // ratio
		float alpha = (maxDist - el.m_pos.distToMeters(game::localPlayer->absOrigin())) / maxDist; // alpha fading per distance
		if (alpha < 0.1f)
			continue;

		if (constexpr float maxDiff = 1.0f; diff < maxDiff) // fading effect
			alpha = (diff / maxDiff) * alpha;

		// again same thing, I don't want to use ent origin but current pos
		auto scaledFont = [=](const float division = 80.0f, const float min = 12.0f, const float max = 30.0f)
		{
			float dist = el.m_pos.distTo(game::localPlayer->absOrigin());
			float fontSize = std::clamp(division / (dist / division), min, max);
			return fontSize;
		};

		float rad = scaledFont(50.0f, 3.0f, 8.0f);
		imRender.drawCircleFilled(elPos.x, elPos.y, rad, 32,
			config.get<CfgColor>(vars.cSoundEsp).getColor().getColorEditAlpha(alpha));

		float distFromMiddle = std::round(std::sqrt((elPos.x - x) * (elPos.x - x) + (elPos.y - y) * (elPos.y - y)));

		if (distFromMiddle < maxDistLine)
		{
			if (!m_bestStep.m_player || distFromMiddle < m_bestStep.m_maxPixels)
			{
				m_bestStep.m_player = el.m_player;
				m_bestStep.m_pos = el.m_pos;
				m_bestStep.m_timeToPrint = diff;
				m_bestStep.m_maxPixels = distFromMiddle;
				// better not
				/*bestStep.m_fontSize = scaledFont(50.0f, 10.0f, 18.0f);*/
			}
		}

		i++;
	}
}

void SoundDraw::draw()
{
	float x = globals::screenX / 2.0f;
	float y = globals::screenY / 2.0f;

	if (m_bestStep.m_player)
	{
		if (Vector2D pos; imRender.worldToScreen(m_bestStep.m_pos, pos))
		{
			std::string_view place = m_bestStep.m_player->m_szLastPlaceName();
			if (place.empty())
				place = XOR("Unknown");
			std::string nameText = FORMAT(XOR("{} -> {} [{:.1f}m]"), m_bestStep.m_player->getName(),
				place, game::localPlayer->absOrigin().distToMeters(m_bestStep.m_pos));
			std::string timeText = FORMAT(XOR("Time left {:.1f}s"), m_bestStep.m_timeToPrint);

			x = globals::screenX / 2.5f;

			float textSize = std::max(
				imRender.getTextSize(ImFonts::tahoma14, timeText).x,
				imRender.getTextSize(ImFonts::tahoma14, nameText).x);

			static float fontSize = ImFonts::tahoma14->FontSize + 2.0f;

			imRender.text(x, y, ImFonts::tahoma14, timeText, false, Colors::White);
			imRender.text(x, y - fontSize, ImFonts::tahoma14, nameText, false, Colors::White);
			imRender.drawLine(x, y, x + textSize, y, config.get<CfgColor>(vars.cStepLine).getColor());
			imRender.drawLine(x + textSize, y, pos.x, pos.y, config.get<CfgColor>(vars.cStepLine).getColor());
		}

		m_bestStep.m_player = nullptr;
	}
}

void SoundDraw::pushSteps(IGameEvent* event)
{
	if (!config.get<bool>(vars.bSoundEsp))
		return;

	auto who = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!who)
		return;

	if (!who->isPlayer())
		return;

	if (who->isDormant())
		return;

	StepData_t step{ who, who->absOrigin(), interfaces::globalVars->m_curtime + config.get<float>(vars.fStepTime) };
	m_steps.at(step.m_player->getIndex()).push_back(step);
}