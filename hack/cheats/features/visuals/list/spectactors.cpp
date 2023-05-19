#include "spectactors.hpp"

#include <cheats/features/cache/cache.hpp>
#include <cheats/game/game.hpp>
#include <config/vars.hpp>

#include <SDK/structs/Entity.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <array>

#include <cheats/hooks/present.hpp>
#include <cheats/hooks/unkownRoundEnd.hpp>

namespace
{
	struct SpectactorHandler : hooks::Present
	{
		SpectactorHandler()
		{
			this->registerRun(spectactor::draw);
		}
	} specHandler;

	struct SpectactorReset : hooks::UnknownRoundEnd
	{
		SpectactorReset()
		{
			this->registerRun(spectactor::reset);
		}
	} specReset;
}

namespace spectactor
{
	std::unordered_map<ObserverTypes, std::string_view> modeToString
	{
		{ OBS_MODE_NONE, "None" },
		{ OBS_MODE_DEATHCAM, "Deathcam" },
		{ OBS_MODE_FREEZECAM, "Freezecam" },
		{ OBS_MODE_FIXED, "Fixed" },
		{ OBS_MODE_IN_EYE, "In-eye" },
		{ OBS_MODE_CHASE, "Chase" },
		{ OBS_MODE_POI, "Poi" },
		{ OBS_MODE_ROAMING, "Roaming" }
	};

	std::vector<std::pair<std::string, ObserverTypes>> allSpecs;
}

void spectactor::reset()
{
	allSpecs.clear();
}

void spectactor::draw()
{
	if (!vars::misc->spectactorList->enabled)
		return;

	if (!game::isAvailable())
		return;

	allSpecs.clear();

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		auto observer = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientFromHandle(ent->m_hObserverTarget()));
		if (!observer)
			continue;

		if (observer == game::localPlayer)
			continue;

		if(const auto type = observer->m_iObserverMode() != ObserverTypes::OBS_MODE_NONE)
			allSpecs.emplace_back(observer->getName(), observer->m_iObserverMode());
	}

	if (allSpecs.empty())
		return;

	if (ImGui::Begin("Spectactors", &vars::misc->spectactorList->enabled, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration))
	{	
		for (size_t i = 0; auto [name, mode] : allSpecs)
		{
			ImGui::TextUnformatted(std::format("{} | {}", name, modeToString.at(mode)).c_str());
			if (i != allSpecs.size())
				ImGui::Separator();

			i++;
		}

		ImGui::End();
	}
}