#include "spectactors.hpp"

#include <cheats/features/cache/cache.hpp>
#include <cheats/game/game.hpp>
#include <config/vars.hpp>

#include <SDK/structs/Entity.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <array>

void SpectactorList::reset()
{
	m_specs.clear();
}

void SpectactorList::draw()
{
	if (!vars::misc->spectactorList->enabled)
		return;

	if (!game::isAvailable())
		return;

	m_specs.clear();

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		auto observer = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientFromHandle(ent->m_hObserverTarget()));
		if (!observer)
			continue;

		if (observer == game::localPlayer)
			continue;

		if(const auto type = observer->m_iObserverMode() != ObserverTypes::OBS_MODE_NONE)
			m_specs.emplace_back(observer->getName(), observer->m_iObserverMode());
	}

	if (m_specs.empty())
		return;

	if (ImGui::Begin("Spectactors", &vars::misc->spectactorList->enabled, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration))
	{
		static std::array tableNames
		{
			std::pair<std::string_view, int>{ "Name", ImGuiTableColumnFlags_WidthFixed },
			std::pair<std::string_view, int>{ "Mode", ImGuiTableColumnFlags_WidthFixed }
		};

		if (ImGui::BeginTable("##speclist", tableNames.size(),
			ImGuiTableFlags_Borders | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable /*| ImGuiTableFlags_Sortable*/))
		{
			for (const auto [name, flags] : tableNames)
				ImGui::TableSetupColumn(name.data(), flags);

			for (auto [name, mode] : m_specs)
			{
				ImGui::TableNextRow();

				if (ImGui::TableNextColumn())
					ImGui::TextUnformatted(name.c_str());

				if (ImGui::TableNextColumn())
					ImGui::TextUnformatted(m_modeString.at(mode).data());
			}

			ImGui::EndTable();
		}
		ImGui::End();
	}
}