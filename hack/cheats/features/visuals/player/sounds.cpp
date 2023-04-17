#include "sounds.hpp"

#include "../../events/events.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <SDK/structs/Entity.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <imgui_internal.h>

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/333825-bloodhound-inspired-legit-csgo-esp.html
void SoundDraw::findBest(Player_t* ent)
{
	if (!vars::visuals->sound->enabled)
		return;

	const int index = ent->getIndex();

	const float x = globals::screenX / 2.0f;
	const float y = globals::screenY / 2.0f;
	const float maxDist = vars::visuals->sound->maxDist;
	const float maxDistLine = vars::visuals->sound->maxDistLine;

	for (size_t i = 0; const auto & el : m_steps.at(index))
	{
		float diff = el.m_expire - memory::interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_steps.at(index).erase(m_steps.at(index).begin() + i);
			continue;
		}

		ImVec2 elPos;
		if (!ImRender::worldToScreen(el.m_pos, elPos))
			continue;

		//float scale = diff / config.get<float>(vars.fStepTime); // ratio
		float alpha = (maxDist - el.m_pos.distToMeters(game::localPlayer->absOrigin())) / maxDist; // alpha fading per distance
		if (alpha < 0.1f)
			continue;

		if (constexpr float maxDiff = 1.0f; diff < maxDiff) // fading effect
			alpha = (diff / maxDiff) * alpha;

		const float rad = game::getScaledFont(el.m_pos, game::localPlayer->absOrigin(), 50.0f, 3.0f, 8.0f);;
		ImRender::drawCircleFilled(elPos.x, elPos.y, rad, 32,
			vars::visuals->sound->color().getColorEditAlpha(alpha));

		const ImVec2 fromScreen = ImVec2{ elPos.x - x,  elPos.y - y };
		const float distFromMiddle = std::round(std::sqrt(ImLengthSqr(fromScreen)));

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
	const float y = globals::screenY / 2.0f;

	if (m_bestStep.m_player)
	{
		if (ImVec2 pos; ImRender::worldToScreen(m_bestStep.m_pos, pos))
		{
			std::string_view place = m_bestStep.m_player->m_szLastPlaceName();
			if (place.empty())
				place = "Unknown";
			const std::string nameText = std::format("{} -> {} [{:.1f}m]", m_bestStep.m_player->getName(),
				place, game::localPlayer->absOrigin().distToMeters(m_bestStep.m_pos));
			const std::string timeText = std::format("Time left {:.1f}s", m_bestStep.m_timeToPrint);

			x = globals::screenX / 2.5f;

			const float textSize = std::max(
				ImRender::getTextSize(ImRender::fonts::tahoma14, ImRender::fonts::tahoma14->FontSize, timeText).x,
				ImRender::getTextSize(ImRender::fonts::tahoma14, ImRender::fonts::tahoma14->FontSize, nameText).x);

			static float fontSize = ImRender::fonts::tahoma14->FontSize + 2.0f;

			ImRender::text(x, y, ImRender::fonts::tahoma14, timeText, false, Colors::White);
			ImRender::text(x, y - fontSize, ImRender::fonts::tahoma14, nameText, false, Colors::White);
			ImRender::drawLine(x, y, x + textSize, y, vars::visuals->sound->colorLine());
			ImRender::drawLine(x + textSize, y, pos.x, pos.y, vars::visuals->sound->colorLine());
		}

		m_bestStep.m_player = nullptr;
	}
}

void SoundDraw::pushSteps(Player_t* ent)
{
	if (!ent->isOtherTeam(game::localPlayer()))
		return;

	StepData_t step{ ent, ent->absOrigin(), memory::interfaces::globalVars->m_curtime + vars::visuals->sound->time };
	m_steps.at(step.m_player->getIndex()).push_back(step);
}