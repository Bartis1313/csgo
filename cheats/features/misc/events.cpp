#include "events.hpp"

#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../visuals/player.hpp"
#include "../misc/misc.hpp"
#include "../visuals/world.hpp"
#include "../misc/logger.hpp"
#include "../visuals/radar.hpp"

#include "../../game.hpp"
#include "../../globals.hpp"
#include "../../../utilities/console/console.hpp"
#include "../../../config/vars.hpp"

void Events::add(const std::string& eventName, const std::function<void(IGameEvent*)>& fun)
{
	interfaces::eventManager->addListener(&events, eventName.c_str());
	std::function<void(IGameEvent*)> m;
	m_events.emplace_back(StructEvent_t{ &events, eventName, fun });
}

void Events::init()
{
	add(XOR("player_death"), [](IGameEvent*)
		{
			//logger.push(Logger::Log_t("died", Colors::White, interfaces::globalVars->m_curtime + 3.0f));
		}
	);
	add(XOR("round_start"), [](IGameEvent*)
		{
			globals::shotsFired = 0, globals::shotsHit = 0, globals::shotsHead = 0;
			radar.setInited(false);
		}
	);
	add(XOR("player_hurt"), [](IGameEvent* e)
		{
			logger.add(Logger::Log_t(XOR("hurt event"), Colors::White, interfaces::globalVars->m_curtime + config.get<float>(vars.fLogMaxTime)));
			misc.playHitmarker(e);
		}
	);
	add(XOR("weapon_fire"), [](IGameEvent* e)
		{
			auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(e->getInt(XOR("userid"))));
			if (!attacker)
				return;

			if (attacker == game::localPlayer)
				globals::shotsFired++;
		}
	);
	add(XOR("bullet_impact"), [](IGameEvent* e)
		{
			auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(e->getInt(XOR("userid"))));
			if (!attacker)
				return;

			if (attacker != game::localPlayer)
				return;

			auto local = reinterpret_cast<Player_t*>(attacker);

			float x = e->getFloat(XOR("x"));
			float y = e->getFloat(XOR("y"));
			float z = e->getFloat(XOR("z"));

			Vector src = local->getEyePos();
			Vector dst = { x, y, z };

			world.pushLocalImpacts({ src, dst, interfaces::globalVars->m_curtime + config.get<float>(vars.fDrawLocalSideImpacts) });
			misc.drawBulletTracer(src, dst);
		}
	);
	add(XOR("bomb_exploded"), [](IGameEvent*)
		{
			world.setBombEnt(nullptr);
		}
	);
	add(XOR("bomb_planted"), [](IGameEvent* e)
		{
			auto who = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(e->getInt(XOR("userid")))));
			if (!who)
				return;

			world.setWhoPlanted(who->getName());
		}
	);
	add(XOR("player_footstep"), [](IGameEvent* e)
		{
			auto who = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(e->getInt(XOR("userid")))));
			if (!who)
				return;

			if (!who->isPlayer())
				return;

			if (who->isDormant())
				return;

			visuals.pushStep(Visuals::StepData_t{ who, who->absOrigin(), interfaces::globalVars->m_curtime + config.get<float>(vars.fStepTime) });
		}
	);

	console.log(TypeLogs::LOG_INFO, XOR("events init"));
}

void Events::FireGameEvent(IGameEvent* event)
{
	if (auto it = std::find_if(m_events.cbegin(), m_events.cend(), [name = event->getName()](const StructEvent_t& el)
	{
		return el.m_name == name;
	}); it != m_events.cend())
	{
		it->m_fun(event);
	}
}

void Events::shutdown() const
{
	interfaces::eventManager->removeListener(&events);
}