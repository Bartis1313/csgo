#include "playerlist.hpp"

#include <cheats/features/cache/cache.hpp>
#include <cheats/game/game.hpp>
#include <config/vars.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/Enums.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/animations.hpp>
#include <cheats/features/blacklist/blacklist.hpp>
#include <render/Color.hpp>

#include <imgui.h>
#include <magic_enum.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <ranges>
#include <iostream>

struct TableStruct
{
	constexpr TableStruct(const std::string_view name, const int flags, bool* cfg = nullptr)
		: name{ name }, flags{ flags }, cfg{ cfg }
	{}

	std::string_view name;
	int flags;
	bool* cfg = nullptr; // some are default always
};

void PlayerList::draw()
{
	if (!vars::misc->playerList->enabled)
		return;

	if (!game::isAvailable())
		return;

	// ImGui demo: Tables/Borders
	if (ImGui::Begin("PlayerList", &vars::misc->playerList->enabled))
	{
		static std::array tableNames
		{
			TableStruct{ "Name", ImGuiTableColumnFlags_NoHide },
			TableStruct{ "Health", ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->health },
			TableStruct{ "Money", ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->money },
			TableStruct{ "Team", ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->teamID },
			TableStruct{ "Place", ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->lastPlace },
			TableStruct{ "Blacklist", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize }
		};

		if (ImGui::BeginTable("", tableNames.size(),
			ImGuiTableFlags_Borders | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable /*| ImGuiTableFlags_Sortable*/))
		{
			for (size_t i = 0; const auto [name, flags, cfg] : tableNames)
			{
				ImGui::TableSetupColumn(name.data(), flags);
				if (cfg)
					ImGui::TableSetColumnEnabled(i, *cfg);

				i++;
			}
			ImGui::TableHeadersRow();

			auto players = EntityCache::getCache(EntCacheType::PLAYER);
			std::ranges::sort(players,
				[](const EntityCache::HolderData& lhs, const EntityCache::HolderData& rhs)
				{
					auto lhsEnt = reinterpret_cast<Player_t*>(lhs.ent);
					auto rhsEnt = reinterpret_cast<Player_t*>(rhs.ent);
					bool lhsIsOtherTeam = lhsEnt->isOtherTeam(game::localPlayer());
					bool rhsIsOtherTeam = rhsEnt->isOtherTeam(game::localPlayer());

					if (lhsIsOtherTeam ^ rhsIsOtherTeam)
						return lhsIsOtherTeam;

					return false;
				}
			);

			for (size_t i = 0; auto [entity, idx, classID] : players)
			{
				auto ent = reinterpret_cast<Player_t*>(entity);

				if (ent == game::localPlayer)
					continue;

				// check once again, will be faster than extra vector
				bool isEnemy = ent->isOtherTeam(game::localPlayer());

				// allow buttons to be executed for ONLY selected player
				ImGui::PushID(i);

				ImGui::TableNextRow();

				if (ImGui::TableNextColumn())
				{
					ImVec4 color = isEnemy ? ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f } : ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f };
					ImGui::PushStyleColor(ImGuiCol_Text, color);
					ImGui::TextUnformatted(ent->getName().c_str());
					ImGui::PopStyleColor();
				}

				if (ImGui::TableNextColumn())
				{
					const auto health = ent->isDormant()
						? CacheFields::getCachedFields().at(idx).m_health
						: ent->m_iHealth();
					ImVec4 color;
					std::memcpy(&color, Color::healthBased(health).data(), 4 * sizeof(float));

					ImGui::PushStyleColor(ImGuiCol_Text, color);
					std::string text = health == 0 ? "DEAD" : std::format("{}", health);
					ImGui::TextUnformatted(text.c_str());
					ImGui::PopStyleColor();

					if (health)
					{
						ImGui::SameLine();
						ImGui::TextUnformatted("HP");
					}
				}

				if (ImGui::TableNextColumn())
				{
					ImGui::TextUnformatted(std::format("{}$", ent->m_iAccount()).c_str());
				}

				if (ImGui::TableNextColumn())
				{
					ImGui::TextUnformatted(std::format("{} ({})", ent->m_iTeamNum(),
						magic_enum::enum_names<TeamID>().at(ent->m_iTeamNum())).c_str());
				}

				if (ImGui::TableNextColumn())
				{
					ImGui::TextUnformatted(ent->m_szLastPlaceName());
				}

				if (ImGui::TableNextColumn())
				{
					enum class BlacklistAction { ADD, REMOVE };

					const auto blacklist = !g_Blacklist->isBlacklisted(ent)
						? std::make_pair(std::format("Add##{}", idx), BlacklistAction::ADD)
						: std::make_pair(std::format("Delete##{}", idx), BlacklistAction::REMOVE);

					const auto [title, isOn] = blacklist;

					if (ImGui::Animations::Button(title.c_str(), ImVec2{ -std::numeric_limits<float>::min(), 0.0f }))
					{	
						isOn == BlacklistAction::ADD
							? g_Blacklist->add(ent)
							: g_Blacklist->remove(ent);
					}
				}

				ImGui::PopID();

				i++;
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}
}
