#include "playerlist.hpp"

#include <features/cache/cache.hpp>
#include <game/game.hpp>
#include <config/vars.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/Enums.hpp>
#include <menu/GUI-ImGui/imguiaddons.hpp>

#include <dependencies/ImGui/imgui.h>
#include <dependencies/magic_enum.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <ranges>

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
	if (ImGui::Begin(XOR("PlayerList"), &vars::misc->playerList->enabled))
	{
		constexpr size_t columnsCount = 6;

		if (ImGui::BeginTable(XOR("PlayerList Table"), columnsCount,
			ImGuiTableFlags_Borders | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable /*| ImGuiTableFlags_Sortable*/))
		{
			static std::array tableNames
			{
				TableStruct{ XOR("Name"), ImGuiTableColumnFlags_NoHide },
				TableStruct{ XOR("Health"), ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->health },
				TableStruct{ XOR("Money"), ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->money },
				TableStruct{ XOR("Team"), ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->teamID },
				TableStruct{ XOR("Place"), ImGuiTableColumnFlags_WidthFixed, &vars::misc->playerList->lastPlace },
				TableStruct{ XOR("Blacklist"), ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize }
			};

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
					if (!ent->isDormant())
					{
						auto health = ent->m_iHealth();
						ImVec4 color;
						std::memcpy(&color, Color::healthBased(health).data(), 4 * sizeof(float));

						ImGui::PushStyleColor(ImGuiCol_Text, color);
						std::string text = health == 0 ? XOR("DEAD") : FORMAT(XOR("{}"), ent->m_iHealth());
						ImGui::TextUnformatted(text.c_str());
						ImGui::PopStyleColor();

						if (health)
						{
							ImGui::SameLine();
							ImGui::TextUnformatted(XOR("HP"));
						}
					}
					else
					{
						ImGui::TextUnformatted(XOR("UNK")); // or maybe cache this?
					}
				}

				if (ImGui::TableNextColumn())
				{
					ImGui::TextUnformatted(FORMAT(XOR("{}$"), ent->m_iAccount()).c_str());
				}

				if (ImGui::TableNextColumn())
				{
					ImGui::TextUnformatted(FORMAT(XOR("{} ({})"), ent->m_iTeamNum(),
						magic_enum::enum_names<TeamID>().at(ent->m_iTeamNum())).c_str());
				}

				if (ImGui::TableNextColumn())
				{
					ImGui::TextUnformatted(ent->m_szLastPlaceName());
				}

				if (ImGui::TableNextColumn())
				{
					// TODO: finish when blacklist done, pseudo
					/*enum class BlacklistAction { ADD, REMOVE };

					std::pair<const char*, BlacklistAction> blacklist = g_Blacklist->isBlacklisted(guid)
						? std::make_pair(XOR("Add"), BlacklistAction::ADD)
						: std::make_pair(XOR("Delete"), BlacklistAction::REMOVE);

					const auto [title, isOn] = blacklist;

					if (ImGui::Animated::Button(title))
					{
						isOn == BlacklistAction::ADD
							? g_Blacklist->add(guid)
							: g_Blacklist->remove(guid);
					}*/

					// lazy method
					//const float goodWidth = ImGui::CalcTextSize(XOR("Blacklist")).x;

					//ImGui::Button()

					// maybe add here isEnemy check,
					// or leave it, so some annoying teammate can be stored in blacklist file
					ImGui::Animated::Button(XOR("Test"), ImVec2(-std::numeric_limits<float>::min(), 0.0f));
				}

				ImGui::PopID();

				i++;
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}
}
