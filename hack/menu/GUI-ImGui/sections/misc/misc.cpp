#include "misc.hpp"

#include "../common.hpp"

#include "../../selections.hpp"
#include <cheats/features/visuals/world/skybox.hpp>
#include <cheats/features/visuals/world/ambient.hpp>
#include <cheats/features/misc/disable/convars.hpp>

void tabs::misc::draw()
{
	ImGui::Columns(2, nullptr, false);

	ImGui::BeginGroupPanel("Locals", ImGui::GetContentRegionAvail());
	{
		ImGui::Checkbox("Motion blur", &vars::misc->motionBlur->enabled);
		if (vars::misc->motionBlur->enabled)
		{
			ImGui::Checkbox("Forward##Motion Blur", &vars::misc->motionBlur->forward);
			ImGui::SliderFloat("Failling intensity##Motion Blur", &vars::misc->motionBlur->fallingIntensity, 0.0f, 5.0f);
			ImGui::SliderFloat("Falling max##Motion Blur", &vars::misc->motionBlur->fallingMax, 0.0f, 30.0f);
			ImGui::SliderFloat("Falling min##Motion Blur", &vars::misc->motionBlur->fallingMin, 0.0f, 30.0f);
			ImGui::SliderFloat("Strength##Motion Blur", &vars::misc->motionBlur->strength, 0.0f, 20.0f);
			ImGui::SliderFloat("Roll intensity##Motion Blur", &vars::misc->motionBlur->rollIntensity, 0.0f, 1.0f);
		}

		ImGui::SliderFloat("FOV local", &vars::misc->fov->value, -50.0f, 50.0f);
		ImGui::Checkbox("Third Person", &vars::misc->thirdp->enabled);
		if (vars::misc->thirdp->enabled)
		{
			ImGui::SameLine();
			ImGui::Hotkey("ThirdP", &vars::keys->thirdP);
			ImGui::SliderFloat("Distance##thirdp", &vars::misc->thirdp->distance, 1.0f, 500.0f);
			ImGui::SliderFloat("Extra X##thirdp", &vars::misc->thirdp->x, -180.0f, 180.0f);
			ImGui::SliderFloat("Extra Y##thirdp", &vars::misc->thirdp->y, -180.0f, 180.0f);
		}

		ImGui::Checkbox("Latency##enabled", &vars::misc->fakeLatency->enabled);
		if (vars::misc->fakeLatency->enabled)
		{
			ImGui::SliderFloat("Fake latency ms", &vars::misc->fakeLatency->amount, 0.0f, 200.0f);
		}

		ImGui::Checkbox("Bunnyhop", &vars::misc->bunnyHop->enabled);
		if (vars::misc->bunnyHop->enabled)
		{
			ImGui::SliderInt("Hitchance##bunny", &vars::misc->bunnyHop->chance, 0, 100, "%i%%");
		}
		ImGui::Combo("Autostrafe", &vars::misc->bunnyHop->indexStrafe, magic_enum::enum_names_pretty<MovementStraferMode>());
		ImGui::Combo("Crosshair type", &vars::misc->crosshair->index, magic_enum::enum_names_pretty<CrossHairTypes>());
	}
	ImGui::EndGroupPanel();

	ImGui::BeginGroupPanel("Windows", ImGui::GetContentRegionAvail());
	{
		ImGui::Checkbox("FPS Plot", &vars::misc->plots->enabledFps);
		ImGui::Checkbox("Velocity Plot", &vars::misc->plots->enabledVelocity);
		ImGui::SameLine();
		ImGui::Checkbox("Run transparent", &vars::misc->plots->transparencyVelocity);
		ImGui::SameLine();
		ImGui::HelpMarker("Will add some flags!\nEg: no resize");

		ImGui::Checkbox("Draw misc info", &vars::misc->info->enabled);
		ImGui::Checkbox("Playerlist##checkbox", &vars::misc->playerList->enabled);
		ImGui::SameLine();
		ImGui::PopupButton("##playerlistpop", []()
			{
				ImGui::Checkbox("Health##playerlist", &vars::misc->playerList->health);
				ImGui::Checkbox("Money##playerlist", &vars::misc->playerList->money);
				ImGui::Checkbox("Team##playerlist", &vars::misc->playerList->teamID);
				ImGui::Checkbox("Place##playerlist", &vars::misc->playerList->lastPlace);
			});
		
		ImGui::Checkbox("Spectactors##specson", &vars::misc->spectactorList->enabled);
	}
	ImGui::EndGroupPanel();

	ImGui::NextColumn();
	{
		ImGui::BeginGroupPanel("Extras", ImGui::GetContentRegionAvail());
		{
			ImGui::Checkbox("Game logs", &vars::misc->logs->enabled);
			ImGui::SameLine();
			ImGui::SliderFloat("Time##Gamelogs", &vars::misc->logs->time, 1.0f, 10.0f);
			ImGui::Checkbox("FreeCam", &vars::misc->freeCam->enabled);
			ImGui::SameLine();
			ImGui::Hotkey("##fc", &vars::keys->freeCam);
			ImGui::SameLine();
			ImGui::SliderFloat("Speed##fc", &vars::misc->freeCam->speed, 1.0f, 20.0f);
			ImGui::Checkbox("MirrorCam", &vars::misc->mirrorCam->enabled);
			ImGui::SameLine();
			ImGui::Checkbox("On key##mcam", &vars::misc->mirrorCam->onKey);
			ImGui::SameLine();
			ImGui::Hotkey("##mcam", &vars::keys->mirrorCam);
			ImGui::Checkbox("Flashlight", &vars::misc->flashLight->enabled);
			ImGui::SameLine();
			ImGui::Hotkey("##flashl key", &vars::keys->flashLight);
			ImGui::SameLine();
			ImGui::Checkbox("Big mode", &vars::misc->flashLight->bigMode);
			ImGui::SliderFloat("Flashlight FOV", &vars::misc->flashLight->fov, 1.0f, 100.0f);
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Removals", ImGui::GetContentRegionAvail());
		{
			ImGui::Checkbox("No scope", &vars::misc->scope->enabled);
			ImGui::Checkbox("Disable Interpolation", &vars::misc->disableItems->interpolate);
			ImGui::SliderFloat("Flashbang %", &vars::misc->disableItems->flashPercentage, 0.0f, 1.0f);
			if(ImGui::Button("Disable cvar locks"))
				convars::run();
		}
		ImGui::EndGroupPanel();
	}

	ImGui::Columns();
}