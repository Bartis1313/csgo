#include "aim.hpp"

#include "../common.hpp"

void tabs::aim::draw()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 backupPadding = style.FramePadding;
	const float width = ImGui::GetContentRegionAvail().x;

	// remove tab underline
	ImGui::PushStyleColor(ImGuiCol_TabActive, Color::U32(Colors::Blank));
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Color::U32(Colors::Blank));

	if (ImGui::BeginTabBar("tabbaraim"))
	{
		const static auto weaponNames = magic_enum::enum_names_pretty<AimbotWeapon>();

		style.FramePadding = { width / weaponNames.size(), backupPadding.y }; // still this is clamped by imgui in tabs
		ImGui::PopStyleColor(2);

		for (size_t i = 0; const auto& name : weaponNames)
		{
			if (ImGui::BeginTabItem(name.c_str()))
			{
				style.FramePadding = backupPadding;

				auto& weapons = vars::aim->weapons;
				auto& cfg = weapons.at(i);

				ImGui::Columns(2, nullptr, false);

				ImGui::BeginGroupPanel("Aimbot", ImGui::GetContentRegionAvail());
				{
					ImGui::Checkbox("Enabled##aim", &cfg.enabled);

					ImGui::BeginDisabled(!cfg.enabled);

					ImGui::Checkbox("Use key", &cfg.useKey);
					ImGui::SameLine();
					ImGui::HelpMarker("Using key will let you select custom key and select states of it\nRecommended to use");
					if (cfg.useKey)
					{
						ImGui::SameLine();
						ImGui::Hotkey("Key##aim", &cfg.key);
					}

					ImGui::SliderFloat("Fov##aim", &cfg.fov, 0.0f, 50.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
					ImGui::Combo("Hitboxes##aim", &cfg.aimSelection, magic_enum::enum_names_pretty<AimbotHitboxes>());
					ImGui::Combo("Method##aim", &cfg.methodAim, magic_enum::enum_names_pretty<AimbotMethod>());
					ImGui::Checkbox("Delay##aim", &cfg.aimDelay);
					if (cfg.aimDelay)
					{
						ImGui::SliderFloat("Delay ms##aim", &cfg.aimDelayVal, 0.0f, 400.0f);
					}
					ImGui::Checkbox("Aim at Backtrack", &cfg.aimBacktrack);
					ImGui::SameLine();
					ImGui::HelpMarker("Will aim at middle of records!");


					ImGui::SliderFloat("Multiply##aim", &cfg.frametimeMulttiply, 0.0f, 30.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
					ImGui::SameLine();
					ImGui::HelpMarker("Scales with the frametime, more = faster");

					ImGui::Checkbox("Smoke check##Aimbot", &cfg.smokeCheck);
					ImGui::SliderFloat("Flash limit##Aimbot", &cfg.flashLimit, 0.0f, 255.0f);

					ImGui::EndDisabled();
				}
				ImGui::EndGroupPanel();

				ImGui::NextColumn();

				ImGui::BeginGroupPanel("Triggerbot", ImGui::GetContentRegionAvail());
				{
					ImGui::Checkbox("Enabled##trigger", &cfg.triggerbot);
					if (cfg.triggerbot)
					{
						ImGui::SliderFloat("Delay##trigger", &cfg.triggerbotDelay, 0.0f, 200.0f);
					}
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel("RCS", ImGui::GetContentRegionAvail());
				{
					ImGui::Checkbox("Enabled##RCS", &cfg.rcs);
					if (cfg.rcs)
					{
						ImGui::SliderFloat("Pitch##Rcsx", &cfg.rcsX, 0.0f, 1.0f);
						ImGui::SliderFloat("Yaw##Rcsy", &cfg.rcsY, 0.0f, 1.0f);
					}
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel("Extra", ImGui::GetContentRegionAvail());
				{
					ImGui::Checkbox("Backtrack##enabled", &vars::backtrack->enabled);
					ImGui::SliderFloat("Backtrack ms", &vars::backtrack->time, 0.0f, 200.0f);
					ImGui::Checkbox("Smoke check##Backtrack", &vars::backtrack->smoke);
					ImGui::SliderFloat("Flash limit##Backtrack", &vars::backtrack->flashLimit, 0.0f, 255.0f);
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel("Drawing", ImGui::GetContentRegionAvail());
				{
					ImGui::Checkbox("Draw fov", &vars::aimPaint->enabledFov);
					if (vars::aimPaint->enabledFov)
					{
						ImGui::SameLine();
						ImGui::ColorPicker("Color##fov", &vars::aimPaint->colorFov);
						ImGui::SameLine();
						ImGui::HelpMarker("Draws a circle representing your aimbot FOV");
						ImGui::Checkbox("Draw aimbot point", &vars::aimPaint->enabledPoint);
						ImGui::SameLine();
						ImGui::ColorPicker("Color##bestpoint", &vars::aimPaint->colorPoint);
					}
				}
				ImGui::EndGroupPanel();

				ImGui::Columns();


				ImGui::EndTabItem();
			}

			i++;
		}

		ImGui::EndTabBar();
	}
}