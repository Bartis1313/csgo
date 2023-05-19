#include "settings.hpp"

#include "../common.hpp"

#include "../background.hpp"
#include <config/config.hpp>
#include <utilities/console/console.hpp>

#include <imgui_stdlib.h>
#include <mutex>

void tabs::settings::draw()
{
	static std::string text = "Your new config name";
	static int currentcfg = 0;

	std::once_flag onceFlag;
	std::call_once(onceFlag, []
		{
			for (size_t i = 0; const auto & el : config::getAllConfigFiles())
			{
				if (el == config::getCfgToLoad())
					currentcfg = i;

				i++;
			}
		});

	ImGui::Columns(2, nullptr, false);

	ImGui::BeginChild("##leftconfig");
	{
		const auto allcfg = config::getAllConfigFiles();

		ImGui::BeginGroupPanel("Config", ImGui::GetContentRegionAvail());
		{
			if (ImGui::InputText("Config name", &text, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (utilities::toLowerCase(text) != config::getDefaultConfigName())
				{
					config::save(text);
					text.clear();
					config::reload();
				}
				else
					console::error("provided config name was same as default");
			}
			ImGui::SameLine();
			ImGui::HelpMarker("Press enter to create new config");
			ImGui::ListBox("All configs", &currentcfg, std::span(allcfg.data(), allcfg.size()));

			static bool dontAskMe = false;
			static bool delayedClose = false; // not instant close for modal 
			if (ImGui::Button("Delete"))
			{
				if (dontAskMe && delayedClose)
				{
					config::deleteCfg(allcfg.at(currentcfg));
					config::reload();
				}
				else
					ImGui::OpenPopup("Delete?");
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Button("Reset asking"))
				{
					dontAskMe = false;
					delayedClose = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("Delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::TextUnformatted(std::format("Are you sure you want to delete {} file?", allcfg.at(currentcfg)).c_str());
				ImGui::Separator();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });
				ImGui::Checkbox("Don't ask again", &dontAskMe);
				ImGui::PopStyleVar();

				if (ImGui::Button("OK", { 120.0f, 0.0f }))
				{
					config::deleteCfg(allcfg.at(currentcfg));
					config::reload();
					ImGui::CloseCurrentPopup();

					if (dontAskMe)
						delayedClose = true;
					else
						delayedClose = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", { 120.0f, 0.0f }))
				{
					ImGui::CloseCurrentPopup();

					if (dontAskMe)
						delayedClose = true;
					else
						delayedClose = false;
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				config::save(allcfg.at(currentcfg));
			}
			ImGui::SameLine();
			if (ImGui::Button("Load"))
			{
				config::load(allcfg.at(currentcfg));
			}
			if (ImGui::Button("Load on start"))
			{
				// this is only saving the load name, nothing more
				config::startSave(allcfg.at(currentcfg));
				console::info("{} will be now loaded config on the start", allcfg.at(currentcfg));
			}
			ImGui::SameLine();
			ImGui::HelpMarker("This config will load on the start");
		}
		ImGui::EndGroupPanel();
	}

	ImGui::EndChild();
	ImGui::NextColumn();

	ImGui::BeginChild("##rigthconfig");
	{
		ImGui::BeginGroupPanel("Config keys", ImGui::GetContentRegionAvail());
		{
			ImGui::Hotkey("Menu key", &vars::keys->menu, false);
			ImGui::Hotkey("Console key", &vars::keys->console, false);
			ImGui::Hotkey("Panic key", &vars::keys->panic, false);
			ImGui::Hotkey("X88 key", &vars::keys->x88Toggle, false);
			ImGui::Checkbox("Enable x88 menu", &vars::keys->enabledX88Menu);
		}
		ImGui::EndGroupPanel();
	}

	ImGui::EndChild();
	ImGui::Columns();
}