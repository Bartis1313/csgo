#include "impacts.hpp"

#include "../../events/events.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/clientHitVerify.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>

#include "../misc/bulletUpdater.hpp"

#include <cheats/hooks/paintTraverse.hpp>

namespace
{
	struct BulletImpactHandler : hooks::PaintTraverse
	{
		BulletImpactHandler()
		{
			this->registerRender(bulletImpacts::draw);
		}
	} bulletImpactHandler;
}

namespace bulletImpacts
{
	struct HitStruct_t
	{
		Vec3 pos{ };
		float expire{ };
	};

	std::vector<HitStruct_t> hits{ };
}

void bulletImpacts::draw()
{
    if (!vars::visuals->world->impacts->enabledClient)
    {
        if (!hits.empty())
            hits.clear();
        return;
    }

    if (!game::isAvailable())
        return;

    for (const auto& el : bulletUpdater::getLastBullets())
    {
        hits.emplace_back(HitStruct_t
            {
                el,
                memory::interfaces::globalVars->m_curtime + vars::visuals->world->impacts->timeClient
            });
    }

    const Color outline = vars::visuals->world->impacts->colorClient();
    const Color fill = vars::visuals->world->impacts->colorClientFill();

    for (size_t i = 0; const auto [pos, expire] : hits)
    {
        float diff = expire - memory::interfaces::globalVars->m_curtime;

        if (diff < 0.0f)
        {
            hits.erase(hits.begin() + i);
            continue;
        }

        ImRender::drawBox3DFilled(pos, 4.0f, 4.0f, outline, fill);

        ++i;
    }
}
