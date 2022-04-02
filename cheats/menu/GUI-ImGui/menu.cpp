#include "menu.hpp"
#include "../../../dependencies/ImGui/imgui.h"
#include "../../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../../dependencies/ImGui/imgui_impl_dx9.h"
#include "../../../utilities/utilities.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/console/console.hpp"
#include "../../../config/vars.hpp"
#include "styles.hpp"

void runStyle(const int idx);

// see the code of dear imgui lib or demo window to change it.
void ImGuiMenu::init()
{
	runStyle(config.get<int>(vars.iStyleMenu));

	ImGuiIO& io = ImGui::GetIO();

	// NEED static here
	static auto p = config.getPathForSave(XOR("window.ini")).string();

	io.IniFilename = p.c_str();
	io.LogFilename = nullptr;

	try
	{
		imRender.init(io);
	}
	catch (const std::runtime_error& err)
	{
		console.log(TypeLogs::LOG_ERR, err.what());
	}
}

void ImGuiMenu::shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	if (ImGui::GetCurrentContext())
		ImGui::DestroyContext();
}

void ImGuiMenu::example()
{
	if (isMenuActive())
	{
		ImGui::ShowDemoWindow(&m_active);
	}
}

#pragma region menu_functionalities
#include "imguiaddons.hpp"
#include "../../../config/vars.hpp"
#include "selections.hpp"

static void renderAimbot()
{
	// using columns, it will be easier, imgui by default recommends begintable(), but in this case columns do stuff like padding each column for us.
	ImGui::Columns(1, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("aimbot.main"), {}, true), ImGuiWindowFlags_AlwaysAutoResize)
		{
			ImGui::BeginGroupPanel(XOR("Aimbot general"), { -1.0f, 0.0f });
			{
				ImGui::Checkbox(XOR("Enabled aimbot"), &config.getRef<bool>(vars.bAimbot));
				ImGui::SliderFloat(XOR("Aimbot FOV"), &config.getRef<float>(vars.fFovAimbot), 0.0f, 50.0f);
				ImGui::Combo(XOR("Aimbot selection"), &config.getRef<int>(vars.iAimbot), selections::aimbotHitboxes);
				ImGui::SliderFloat(XOR("Aimbot smooth"), &config.getRef<float>(vars.fSmooth), 1.0f, 50.0f);
				ImGui::Checkbox(XOR("Enabled RCS"), &config.getRef<bool>(vars.bRCS));
				ImGui::SliderFloat(XOR("RCS percentage"), &config.getRef<float>(vars.fRCS), 0.0f, 100.0f);
				ImGui::Checkbox(XOR("Triggerbot enabled"), &config.getRef<bool>(vars.bTriggerbot));
				ImGui::SliderFloat(XOR("Triggerbot ms"), &config.getRef<float>(vars.fTriggerDelay), 0.0f, 200.0f);
				ImGui::Checkbox(XOR("Draw fov"), &config.getRef<bool>(vars.bDrawFov));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Fov circle color"), &config.getRef<Color>(vars.cDrawFov));
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Draws a circle representing your aimbot FOV"));
				ImGui::Checkbox(XOR("Draw aimbot point"), &config.getRef<bool>(vars.bDrawBestPoint));
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XOR("Backtracking"), { -1.0f, 0.0f });
			{
				if (ImGui::BeginMenuBar())
				{
					ImGui::Text(XOR("Aimbot Section"));
					ImGui::EndMenuBar();
				}

				ImGui::Checkbox(XOR("Enabled backtrack"), &config.getRef<bool>(vars.bBacktrack));
				ImGui::SliderFloat(XOR("Backtrack ms"), &config.getRef<float>(vars.fBacktrackTick), 0.0f, 200.0f);
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
}

static void renderVisuals()
{
	ImGui::Columns(2, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("visuals"), {}, true, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginGroupPanel(XOR("Players"));
			{
				ImGui::Checkbox(XOR("Enabled"), &config.getRef<bool>(vars.bEsp));
				ImGui::Combo(XOR("Boxes mode"), &config.getRef<int>(vars.iEsp), selections::espNames);
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Box color"), &config.getRef<Color>(vars.cBox));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Box color, filled"), &config.getRef<Color>(vars.cBoxFill), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf);
				ImGui::Checkbox(XOR("Health"), &config.getRef<bool>(vars.bDrawHealth));
				ImGui::Checkbox(XOR("Armor"), &config.getRef<bool>(vars.bDrawArmor));
				ImGui::Checkbox(XOR("Infos"), &config.getRef<bool>(vars.bDrawInfos));
				ImGui::Checkbox(XOR("Skeleton"), &config.getRef<bool>(vars.bDrawSkeleton));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Skeleton color"), &config.getRef<Color>(vars.cSkeleton));
				ImGui::Checkbox(XOR("Dlight"), &config.getRef<bool>(vars.bDLight));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Dlight color"), &config.getRef<Color>(vars.cDlight));
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XOR("Chams"));
			{
				ImGui::Checkbox(XOR("Chams enabled"), &config.getRef<bool>(vars.bChams));
				ImGui::Combo(XOR("Chams type"), &config.getRef<int>(vars.iChams), selections::chamsNames);
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Chams color"), &config.getRef<Color>(vars.cChams));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Chams XYZ color"), &config.getRef<Color>(vars.cChamsXYZ));
				ImGui::Checkbox(XOR("Backtrack chams"), &config.getRef<bool>(vars.bBacktrackChams));
				ImGui::SameLine();
				ImGui::Combo(XOR("##"), &config.getRef<int>(vars.iBacktrackChams), selections::btChams);
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Backtrack chams col"), &config.getRef<Color>(vars.cBackTrackChams));
				ImGui::Checkbox(XOR("Enabled glow"), &config.getRef<bool>(vars.bGlow));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Glow color"), &config.getRef<Color>(vars.cGlow));
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		if (ImGui::BeginChild("wpns", {}, true, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginGroupPanel(XOR("Weapons"));
			{
				ImGui::Checkbox(XOR("Enabled##weapons"), &config.getRef<bool>(vars.bDrawWeapon));
				ImGui::ColorPicker(XOR("Color for text"), &config.getRef<Color>(vars.cWeaponText));
				ImGui::SameLine();
				ImGui::Text(XOR("Weapon text"));
				ImGui::ColorPicker(XOR("Reload bar color"), &config.getRef<Color>(vars.cReloadbar));
				ImGui::SameLine();
				ImGui::Text(XOR("Reload bar"));
				ImGui::ColorPicker(XOR("Dropped C4 color"), &config.getRef<Color>(vars.cDrawDroppedC4));
				ImGui::SameLine();
				ImGui::Text(XOR("Dropped C4"));
				ImGui::ColorPicker(XOR("C4 Info"), &config.getRef<Color>(vars.cDrawBomb));
				ImGui::SameLine();
				ImGui::Text(XOR("C4 information"));
				ImGui::ColorPicker(XOR("Draw molotov circle"), &config.getRef<Color>(vars.cMolotovRange));
				ImGui::SameLine();
				ImGui::Text(XOR("Molotov circle"));
			}
			ImGui::EndGroupPanel();

			ImGui::SameLine();

			ImGui::BeginGroupPanel(XOR("Nades"));
			{
				ImGui::Checkbox(XOR("Enabled##nades"), &config.getRef<bool>(vars.bDrawProjectiles));
				ImGui::ColorPicker(XOR("Flashbang color"), &config.getRef<Color>(vars.cFlashBang));
				ImGui::SameLine();
				ImGui::Text(XOR("Flashbang"));
				ImGui::ColorPicker(XOR("Granede color"), &config.getRef<Color>(vars.cGranede));
				ImGui::SameLine();
				ImGui::Text(XOR("Granede"));
				ImGui::ColorPicker(XOR("Molotov color"), &config.getRef<Color>(vars.cMolotov));
				ImGui::SameLine();
				ImGui::Text(XOR("Molotov"));
				ImGui::ColorPicker(XOR("Incediary color"), &config.getRef<Color>(vars.cIncediary));
				ImGui::SameLine();
				ImGui::Text(XOR("Incediary"));
				ImGui::ColorPicker(XOR("Smoke color"), &config.getRef<Color>(vars.cSmoke));
				ImGui::SameLine();
				ImGui::Text(XOR("Smoke"));
				ImGui::ColorPicker(XOR("Decoy color"), &config.getRef<Color>(vars.cDecoy));
				ImGui::SameLine();
				ImGui::Text(XOR("Decoy"));
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::Columns();
}

static void renderMisc()
{
	ImGui::Columns(2, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("Misc"), {}, true, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginGroupPanel(XOR("Misc"));
			{
				ImGui::Combo(XOR("Skybox"), &config.getRef<int>(vars.iSkyBox), selections::skyboxes, {});
				bool& modulateRef = config.getRef<bool>(vars.bModulateColor);
				ImGui::Checkbox(XOR("Modulate world"), &modulateRef);
				if (modulateRef)
				{
					ImGui::ColorPicker(XOR("Modulate world texture"), &config.getRef<Color>(vars.cWorldTexture));
					ImGui::SameLine();
					ImGui::Text(XOR("Modulate world texture"));
					ImGui::ColorPicker(XOR("Modulate world prop"), &config.getRef<Color>(vars.cWorldProp));
					ImGui::SameLine();
					ImGui::Text(XOR("Modulate world prop"));
					ImGui::ColorPicker(XOR("Modulate world sky"), &config.getRef<Color>(vars.cSkyBox));
					ImGui::SameLine();
					ImGui::Text(XOR("Modulate world sky"));
					ImGui::Checkbox(XOR("Remove sky"), &config.getRef<bool>(vars.bRemoveSky));
					//ImGui::SliderFloat(XOR("Shader value"), &config.getRef<float>(vars.fShaderParam), 0.0f, 100.0f);
				}
				ImGui::Checkbox(XOR("Zeus range"), &config.getRef<bool>(vars.bDrawZeusRange));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Zeus color"), &config.getRef<Color>(vars.cZeusRange));
				ImGui::Checkbox(XOR("Sound ESP"), &config.getRef<bool>(vars.bSoundEsp));
				ImGui::SliderFloat(XOR("FOV local"), &config.getRef<float>(vars.fFOV), -50.0f, 50.0f);
				ImGui::Checkbox(XOR("Third Person"), &config.getRef<bool>(vars.bThirdp));
				ImGui::Checkbox(XOR("Enable crosshair"), &config.getRef<bool>(vars.bCrosshair));
				ImGui::SameLine();
				ImGui::Combo(XOR("##"), &config.getRef<int>(vars.iCrosshair), selections::crossHairNames);
				ImGui::Checkbox(XOR("Enemy aiming warn"), &config.getRef<bool>(vars.bAimingWarn));
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XOR("Radar"));
			{
				ImGui::Checkbox(XOR("2D Radar enabled"), &config.getRef<bool>(vars.bRadar));
				ImGui::ColorPicker(XOR("Angle view line"), &config.getRef<Color>(vars.cRadarLine));
				ImGui::SameLine();
				ImGui::Text(XOR("Angle line"));
				ImGui::ColorPicker(XOR("Players color"), &config.getRef<Color>(vars.cRadarPlayer));
				ImGui::SameLine();
				ImGui::Text(XOR("Players"));
				ImGui::SliderFloat(XOR("Thickness"), &config.getRef<float>(vars.fRadarThickness), 0.0f, 20.0f);
				ImGui::SliderFloat(XOR("Length"), &config.getRef<float>(vars.fRadarLenght), 0.0f, 40.0f);
				ImGui::SliderFloat(XOR("Scale"), &config.getRef<float>(vars.fRadarScale), 0.0f, 10.0f);
				ImGui::Checkbox(XOR("Draw out of radar"), &config.getRef<bool>(vars.bRadarRanges));
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("If enemy is out of the radar\nThen icons will still appear but clamped"));
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		if (ImGui::BeginChild(XOR("Miscother"), {}, true, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginGroupPanel(XOR("Misc other"));
			{
				ImGui::Checkbox(XOR("Bunnyhop"), &config.getRef<bool>(vars.bBunnyHop));
				ImGui::Checkbox(XOR("Autostrafe"), &config.getRef<bool>(vars.bAutoStrafe));
				ImGui::Checkbox(XOR("Hitmarker"), &config.getRef<bool>(vars.bDrawHitmarker));
				ImGui::SliderFloat(XOR("Hitmarker time"), &config.getRef<float>(vars.fHitmarkerTime), 0.0f, 2.0f);
				ImGui::Checkbox(XOR("Play hitmarker"), &config.getRef<bool>(vars.bPlayHitmarker));
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Will play the sound after every kill"));
				ImGui::Checkbox(XOR("No scope"), &config.getRef<bool>(vars.bNoScope));
				bool& fpsRef = config.getRef<bool>(vars.bShowFpsPlot);
				ImGui::Checkbox(XOR("FPS Plot"), &fpsRef);
				if (fpsRef)
				{
					ImGui::SameLine();
					ImGui::Checkbox(XOR("FPS Custom"), &config.getRef<bool>(vars.bFPSCustom));
				}
				bool& velRef = config.getRef<bool>(vars.bShowVelocityPlot);
				ImGui::Checkbox(XOR("Velocity Plot"), &velRef);
				if (velRef)
				{
					ImGui::SameLine();
					ImGui::Checkbox(XOR("Velocity Custom"), &config.getRef<bool>(vars.bVelocityCustom));
				}
				ImGui::Checkbox(XOR("Draw misc info"), &config.getRef<bool>(vars.bDrawMiscInfo));
				bool& naderef = config.getRef<bool>(vars.bNadePred);
				ImGui::Checkbox(XOR("Nade pred"), &naderef);
				if (naderef)
				{
					ImGui::SameLine();
					ImGui::Checkbox(XOR("Show always"), &config.getRef<bool>(vars.bNadePredAlways));
				}
				bool& tref = config.getRef<bool>(vars.bRunMovementTrail);
				ImGui::Checkbox(XOR("Movement trail"), &tref);
				if (tref)
				{
					bool& rref = config.getRef<bool>(vars.bMovementRainbow);
					ImGui::Checkbox(XOR("Run rainbow"), &rref);
					if (!rref)
					{
						ImGui::SameLine();
						ImGui::ColorPicker(XOR("Movement trail color"), &config.getRef<Color>(vars.cMovementTrail));
					}
					else
					{
						ImGui::SliderFloat(XOR("Rainbow speed"), &config.getRef<float>(vars.fMovementRainbowSpeed), 0.2f, 10.0f);
					}
					ImGui::SliderFloat(XOR("Trail speed"), &config.getRef<float>(vars.fMovementBeamSpeed), 1.0f, 10.0f);
					ImGui::SliderFloat(XOR("Trail life"), &config.getRef<float>(vars.fMovementLife), 1.0f, 10.0f);
				}
				bool& hatRef = config.getRef<bool>(vars.bHat);
				ImGui::Checkbox(XOR("Hat on local"), &hatRef);
				if (hatRef)
				{
					bool& hatRainbowRef = config.getRef<bool>(vars.bHatRainbow);
					ImGui::Checkbox(XOR("Rainbow hat"), &hatRainbowRef);
					if (!hatRainbowRef)
					{
						ImGui::ColorPicker(XOR("Hat triangle color"), &config.getRef<Color>(vars.cHatTriangle));
						ImGui::SameLine();
						ImGui::Text(XOR("Hat triangle color"));
						ImGui::ColorPicker(XOR("Hat lines color"), &config.getRef<Color>(vars.cHatLine));
						ImGui::SameLine();
						ImGui::Text(XOR("Hat lines color"));
					}
					else
					{
						ImGui::SliderFloat(XOR("Hat rainbow speed"), &config.getRef<float>(vars.fHatSpeed), 0.2f, 10.0f);
					}
					ImGui::SliderFloat(XOR("Hat size"), &config.getRef<float>(vars.fHatSize), -100.0f, 100.0f);
					ImGui::SliderFloat(XOR("Hat radius"), &config.getRef<float>(vars.fHatRadius), 1.0f, 100.0f);
				}

			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::Columns();
}

#include "../../../dependencies/ImGui/imgui_stdlib.h"

static void renderConfig()
{
	static std::string text = XOR("Your new config name");
	static int currentcfg = 0;

	ImGui::Columns(1, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("cfg"), {}, true, ImGuiWindowFlags_AlwaysAutoResize))
		{
			const auto allcfg = config.getAllConfigFiles();

			ImGui::BeginGroupPanel(XOR("Config"));
			{
				if (ImGui::InputText(XOR("Config name"), &text, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					config.save(text);
					text.clear();
					config.reload();
				}
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Press enter to create new config"));
				ImGui::ListBox(XOR("All configs"), &currentcfg, allcfg);
				if (ImGui::Button(XOR("Delete")))
				{
					config.deleteCfg(allcfg.at(currentcfg));
					config.reload();
				}
				ImGui::SameLine();
				if (ImGui::Button(XOR("Save")))
				{
					config.save(allcfg.at(currentcfg), true);
				}
				ImGui::SameLine();
				if (ImGui::Button(XOR("Load")))
				{
					config.load(allcfg.at(currentcfg));
					runStyle(config.get<int>(vars.iStyleMenu));
				}
				if (ImGui::Button(XOR("Load on start")))
				{
					config.getRef<std::string>(vars.sLoadName) = allcfg.at(currentcfg);

					console.log(TypeLogs::LOG_INFO, XOR("{} will be now loaded config on the start"), allcfg.at(currentcfg));

					// yes this is confusing, but this is also simple, perfect solution is to get the line where this var is located and edit it
					// this will be done soon
					config.save(config.getDefaultConfigName(), true);
				}
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("This config will load on the start\nThis button saves default config!"));
				static int testkey = 0x4;
				ImGui::Hotkey(XOR("Testing hotkey!"), &testkey);
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
}

static void runStyle(const int idx)
{
	static std::map<int, std::function<void()>> m = 
	{
		{ 0, &styles::runCherry },
		{ 1, &styles::runDark },
		{ 2, &styles::runClassic },
		{ 3, &styles::runWhite },
	};

	m[idx]();
}

static void renderStyles()
{
	ImGui::Columns(1, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("style"), {}, true, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginGroupPanel(XOR("Style"));
			{
				int& styleRef = config.getRef<int>(vars.iStyleMenu);
				ImGui::ListBox(XOR("Menu styles"), &styleRef, selections::styleNames);
				if (ImGui::Button(XOR("Run")))
				{
					runStyle(styleRef);
				}
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
}

#include "tabrender.hpp"

std::array tabs =
{
	TabRender{ "Aimbot", &renderAimbot },
	TabRender{ "Visuals", &renderVisuals },
	TabRender{ "Misc", &renderMisc },
	TabRender{ "Config", &renderConfig },
	TabRender{ "Style", &renderStyles }
};
#pragma endregion

#include "../../../dependencies/ImGui/imgui_internal.h"

void ImGuiMenu::draw()
{
	if (isMenuActive())
	{
		if (ImGui::Begin(XOR("csgo legit"), &m_active, ImGuiWindowFlags_NoCollapse))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 backupPadding = style.FramePadding;
			float width = ImGui::GetContentRegionAvail().x;

			// remove tab underline
			ImGui::PushStyleColor(ImGuiCol_TabActive, U32(Colors::Blank));
			ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, U32(Colors::Blank));

			if (ImGui::BeginTabBar(XOR("tabbar")))
			{
				style.FramePadding = { width / tabs.size(), backupPadding.y }; // still this is clamped by imgui in tabs
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				for (const auto& el : tabs)
				{
					if (ImGui::BeginTabItem(el.m_name))
					{
						style.FramePadding = backupPadding;
						if (el.funcExist())
							el.m_func();

						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::End();
		}
	}
}