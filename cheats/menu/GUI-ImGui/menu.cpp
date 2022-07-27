#include "menu.hpp"
#include "../../../dependencies/ImGui/imgui.h"
#include "../../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../../dependencies/ImGui/imgui_impl_dx9.h"
#include "../../../utilities/utilities.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/console/console.hpp"
#include "../../../config/vars.hpp"
#include "styles.hpp"
#include "../../features/visuals/world.hpp"

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
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Use key##aimbot"), &config.getRef<bool>(vars.bAimbotUseKey));
				ImGui::SameLine();
				ImGui::Hotkey("", &config.getRef<Key>(vars.kAimbotKey));
				ImGui::SliderFloat(XOR("Aimbot FOV"), &config.getRef<float>(vars.fFovAimbot), 0.0f, 50.0f);
				ImGui::Combo(XOR("Aimbot selection"), &config.getRef<int>(vars.iAimbot), selections::aimbotHitboxes);
				ImGui::SliderFloat(XOR("Aimbot smooth"), &config.getRef<float>(vars.fSmooth), 1.0f, 50.0f);
				ImGui::Checkbox(XOR("Enabled RCS"), &config.getRef<bool>(vars.bRCS));
				ImGui::SliderFloat(XOR("RCS percentage axis"), &config.getRef<float>(vars.fRCSx), 0.0f, 100.0f);
				ImGui::SliderFloat(XOR("RCS percentage yaw"), &config.getRef<float>(vars.fRCSy), 0.0f, 100.0f);
				ImGui::Checkbox(XOR("Triggerbot enabled"), &config.getRef<bool>(vars.bTriggerbot));
				ImGui::SliderFloat(XOR("Triggerbot ms"), &config.getRef<float>(vars.fTriggerDelay), 0.0f, 200.0f);
				ImGui::Checkbox(XOR("Draw fov"), &config.getRef<bool>(vars.bDrawFov));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Fov circle color"), &config.getRef<CfgColor>(vars.cDrawFov));
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Draws a circle representing your aimbot FOV"));
				ImGui::Checkbox(XOR("Draw aimbot point"), &config.getRef<bool>(vars.bDrawBestPoint));
				ImGui::Checkbox(XOR("Delay"), &config.getRef<bool>(vars.bAimbotDelay));
				ImGui::SliderFloat(XOR("Delay ms"), &config.getRef<float>(vars.fAimbotDelay), 0.0f, 800.0f);
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
				ImGui::SameLine();
				ImGui::SliderFloat(XOR("Backtrack ms"), &config.getRef<float>(vars.fBacktrackTick), 0.0f, 200.0f);
				ImGui::Checkbox(XOR("Enabled fake latency"), &config.getRef<bool>(vars.bFakeLatency));
				ImGui::SameLine();
				ImGui::SliderFloat(XOR("Fake latency ms"), &config.getRef<float>(vars.fFakeLatency), 0.0f, 200.0f);
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
				ImGui::Combo(XOR("Boxes mode"), &config.getRef<int>(vars.iEsp), selections::espNames, 40.0f);
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("color##box"), &config.getRef<CfgColor>(vars.cBox));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("filled##box"), &config.getRef<CfgColor>(vars.cBoxFill));
				ImGui::Checkbox(XOR("Outlined"), &config.getRef<bool>(vars.bBoxOutlined));
				if (auto type = config.get<int>(vars.iEsp); type == E2T(BoxTypes::FILLED2D) || type == E2T(BoxTypes::FILLED3D))
				{
					ImGui::Checkbox(XOR("Multi color rect fill"), &config.getRef<bool>(vars.bBoxMultiColor));
					ImGui::SliderFloat(XOR("Rect fill multigradient speed"), &config.getRef<float>(vars.fBoxMultiColor), 0.0f, 15.0f);
				}
				ImGui::Checkbox(XOR("Health"), &config.getRef<bool>(vars.bDrawHealth));
				ImGui::Checkbox(XOR("Armor"), &config.getRef<bool>(vars.bDrawArmor));
				ImGui::Checkbox(XOR("Name"), &config.getRef<bool>(vars.bDrawName));
				ImGui::Checkbox(XOR("Skeleton"), &config.getRef<bool>(vars.bDrawSkeleton));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Skeleton color"), &config.getRef<CfgColor>(vars.cSkeleton));
				bool& soundref = config.getRef<bool>(vars.bSoundEsp);
				ImGui::Checkbox(XOR("Sound ESP"), &soundref);
				if (soundref)
				{
					ImGui::ColorPicker(XOR("Circles color"), &config.getRef<CfgColor>(vars.cSoundEsp));
					ImGui::SliderFloat(XOR("Step time"), &config.getRef<float>(vars.fStepTime), 1.0f, 20.0f);
					ImGui::SliderFloat(XOR("Step max distance"), &config.getRef<float>(vars.fStepMaxDist), 5.0f, 500.0f);
					ImGui::SliderFloat(XOR("Lines info distance"), &config.getRef<float>(vars.fStepMaxLineDist), 2.0f, 200.0f);
					ImGui::SameLine();
					ImGui::HelpMarker(XOR("Max pixels to decide to draw info from centre of screen to the best point"));
					ImGui::ColorPicker(XOR("Lines info color"), &config.getRef<CfgColor>(vars.cStepLine));
				}
				ImGui::Checkbox(XOR("Dlight"), &config.getRef<bool>(vars.bDLight));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Dlight color"), &config.getRef<CfgColor>(vars.cDlight));
				ImGui::SliderFloat(XOR("Dlight radius"), &config.getRef<float>(vars.fDlightRadius), 0.0f, 300.0f);
				ImGui::SliderFloat(XOR("Dlight exponent"), &config.getRef<float>(vars.fDlightExponent), 0.0f, 12.0f);
				ImGui::SliderFloat(XOR("Dlight decay"), &config.getRef<float>(vars.fDlightDecay), 0.0f, 100.0f);
				ImGui::MultiCombo(XOR("Esp flags"), selections::flags, config.getRef<std::vector<bool>>(vars.vFlags));
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XOR("Chams"));
			{
				ImGui::Checkbox(XOR("Chams enabled Players"), &config.getRef<bool>(vars.bChamsPlayers));
				ImGui::SameLine();
				ImGui::Combo(XOR("Chams type Players"), &config.getRef<int>(vars.iChamsPlayers), selections::chamsNames);
				ImGui::ColorPicker(XOR("Chams color Players"), &config.getRef<CfgColor>(vars.cChamsPlayers));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Chams XQZ"), &config.getRef<bool>(vars.bChamsXQZPlayers));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Chams XQZ color"), &config.getRef<CfgColor>(vars.cChamsXQZPlayers));
				
				ImGui::Checkbox(XOR("Chams enabled Weapons"), &config.getRef<bool>(vars.bChamsWeapons));
				ImGui::SameLine();
				ImGui::Combo(XOR("Chams type Weapons"), &config.getRef<int>(vars.iChamsWeapons), selections::chamsNames);
				ImGui::ColorPicker(XOR("Chams color Weapons"), &config.getRef<CfgColor>(vars.cChamsWeapons));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Disable weapon"), &config.getRef<bool>(vars.bChamsWeaponsDisable));

				ImGui::Checkbox(XOR("Chams enabled Arms"), &config.getRef<bool>(vars.bChamsArms));
				ImGui::SameLine();
				ImGui::Combo(XOR("Chams type Arms"), &config.getRef<int>(vars.iChamsArms), selections::chamsNames);
				ImGui::ColorPicker(XOR("Chams color Arms"), &config.getRef<CfgColor>(vars.cChamsArms));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Disable arms"), &config.getRef<bool>(vars.bChamsArmsDisable));
				
				ImGui::Checkbox(XOR("Chams enabled Backtrack"), &config.getRef<bool>(vars.bChamsBacktrack));
				ImGui::SameLine();
				ImGui::Combo(XOR("Chams type Backtrack"), &config.getRef<int>(vars.iChamsBacktrackMode), selections::chamsNames);
				ImGui::ColorPicker(XOR("Backtrack chams col"), &config.getRef<CfgColor>(vars.cChamsBacktrack));
				ImGui::SameLine();
				ImGui::Combo(XOR("Chams style Backtrack"), &config.getRef<int>(vars.iChamsBacktrack), selections::btChams);

				ImGui::Checkbox(XOR("Enabled glow"), &config.getRef<bool>(vars.bGlow));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Glow color"), &config.getRef<CfgColor>(vars.cGlow));
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
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Translate name"), &config.getRef<bool>(vars.bDrawWeaponTranslate));
				ImGui::ColorPicker(XOR("Color for text"), &config.getRef<CfgColor>(vars.cWeaponText));
				ImGui::ColorPicker(XOR("Reload bar color"), &config.getRef<CfgColor>(vars.cReloadbar));
				ImGui::Checkbox(XOR("Bomb info"), &config.getRef<bool>(vars.bDrawBomb));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("C4 info"), &config.getRef<CfgColor>(vars.cBombBackground));
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XOR("Nades"));
			{
				ImGui::Checkbox(XOR("Enabled##nades"), &config.getRef<bool>(vars.bDrawProjectiles));
				ImGui::ColorPicker(XOR("Flashbang color"), &config.getRef<CfgColor>(vars.cFlashBang));

				ImGui::ColorPicker(XOR("Granede color"), &config.getRef<CfgColor>(vars.cGranede));

				ImGui::ColorPicker(XOR("Molotov color"), &config.getRef<CfgColor>(vars.cMolotov));

				ImGui::ColorPicker(XOR("Incediary color"), &config.getRef<CfgColor>(vars.cIncediary));

				ImGui::ColorPicker(XOR("Smoke color"), &config.getRef<CfgColor>(vars.cSmoke));

				ImGui::ColorPicker(XOR("Decoy color"), &config.getRef<CfgColor>(vars.cDecoy));

				ImGui::Checkbox(XOR("Enabled dropped"), &config.getRef<bool>(vars.bDrawDropped));
				ImGui::MultiCombo(XOR("Dropped flags"), selections::droppedFlags, config.getRef<std::vector<bool>>(vars.vDroppedFlags));
				ImGui::ColorPicker(XOR("Dropped color"), &config.getRef<CfgColor>(vars.cDrawDropped));

				ImGui::Checkbox(XOR("##Edit molotov"), &config.getRef<bool>(vars.bEditEffectsMoly));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Edited molotov"), &config.getRef<CfgColor>(vars.cEditMolotov));

				ImGui::Checkbox(XOR("##Edit blood"), &config.getRef<bool>(vars.bEditEffectsBlood));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Edited blood"), &config.getRef<CfgColor>(vars.cEditBlood));

				ImGui::Checkbox(XOR("##Edit smoke"), &config.getRef<bool>(vars.bEditEffectsSmoke));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Edited smoke"), &config.getRef<CfgColor>(vars.cEditSmoke));

				ImGui::Checkbox(XOR("##Enabled molotov circle"), &config.getRef<bool>(vars.bDrawmolotovRange));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Draw molotov circle"), &config.getRef<CfgColor>(vars.cMolotovRange));


				ImGui::Checkbox(XOR("##Enabled smoke circle"), &config.getRef<bool>(vars.bDrawSmoke));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Draw smoke circle"), &config.getRef<CfgColor>(vars.cDrawSmoke));
				ImGui::SameLine();
				
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::Columns();
}

#include "../../features/visuals/radar.hpp"

static void renderMisc()
{
	ImGui::Columns(2, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("Misc"), {}, true, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginGroupPanel(XOR("Misc"));
			{
				const auto customsky = world.getAllCustomSkyBoxes();

				ImGui::Combo(XOR("Skybox"), &config.getRef<int>(vars.iSkyBox), selections::skyboxes);
				ImGui::Combo(XOR("Skybox Custom"), &config.getRef<int>(vars.iCustomSkyBox), customsky);
				ImGui::SameLine();
				if (ImGui::Button(XOR("Reload Custom Skybox")))
				{
					world.reloadCustomSkyboxes();
				}
				bool& modulateRef = config.getRef<bool>(vars.bModulateColor);
				ImGui::Checkbox(XOR("Modulate world"), &modulateRef);
				ImGui::Checkbox(XOR("Remove sky"), &config.getRef<bool>(vars.bRemoveSky));
				if (modulateRef)
				{
					ImGui::ColorPicker(XOR("Modulate world texture"), &config.getRef<CfgColor>(vars.cWorldTexture));

					ImGui::ColorPicker(XOR("Modulate world prop"), &config.getRef<CfgColor>(vars.cWorldProp));

					ImGui::ColorPicker(XOR("Modulate world sky"), &config.getRef<CfgColor>(vars.cSkyBox));
		
					//ImGui::SliderFloat(XOR("Shader value"), &config.getRef<float>(vars.fShaderParam), 0.0f, 100.0f);
				}
				ImGui::Checkbox(XOR("Zeus range"), &config.getRef<bool>(vars.bDrawZeusRange));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Zeus color"), &config.getRef<CfgColor>(vars.cZeusRange));
				ImGui::Checkbox(XOR("Zeus party"), &config.getRef<bool>(vars.bZeusPartyMode));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Zeus tracing"), &config.getRef<bool>(vars.bZeusUseTracing));
				ImGui::SliderFloat(XOR("FOV local"), &config.getRef<float>(vars.fFOV), -50.0f, 50.0f);
				ImGui::Checkbox(XOR("Third Person"), &config.getRef<bool>(vars.bThirdp));
				ImGui::SameLine();
				ImGui::Hotkey(XOR(""), &config.getRef<Key>(vars.kThirdp));
				ImGui::SliderFloat(XOR("Distance##thirdp"), &config.getRef<float>(vars.fThirdpDistance), 1.0f, 500.0f);
				ImGui::SliderFloat(XOR("Extra X##thirdp"), &config.getRef<float>(vars.fThirdpX), -180.0f, 180.0f);
				ImGui::SliderFloat(XOR("Extra Y##thirdp"), &config.getRef<float>(vars.fThirdpY), -180.0f, 180.0f);
				ImGui::Combo(XOR("Crosshair type"), &config.getRef<int>(vars.iCrosshair), selections::crossHairNames);
				ImGui::Checkbox(XOR("Enemy aiming warn"), &config.getRef<bool>(vars.bAimingWarn));
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XOR("Radar"));
			{
				ImGui::Checkbox(XOR("2D Radar enabled"), &config.getRef<bool>(vars.bRadar));
				ImGui::ColorPicker(XOR("Angle view line"), &config.getRef<CfgColor>(vars.cRadarLine));
				ImGui::ColorPicker(XOR("Players color"), &config.getRef<CfgColor>(vars.cRadarPlayer));
				ImGui::SliderFloat(XOR("Thickness"), &config.getRef<float>(vars.fRadarThickness), 0.0f, 20.0f);
				ImGui::SliderFloat(XOR("Length"), &config.getRef<float>(vars.fRadarLenght), 0.0f, 40.0f);
				ImGui::SliderFloat(XOR("Scale"), &config.getRef<float>(vars.fRadarScale), 0.0f, 10.0f);
				ImGui::SliderFloat(XOR("Radar size"), &config.getRef<float>(vars.fRadarSize), 10.0f, 500.0f);
				ImGui::Checkbox(XOR("Draw out of radar"), &config.getRef<bool>(vars.bRadarRanges));
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("If enemy is out of the radar\nThen icons will still appear but clamped"));
				if (ImGui::Button(XOR("Refresh texture manually")))
				{
					radar.manuallyInitTexture();
				}
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Will not for workshop maps\nYou can try forcing the engine to re-render by pressing escape few times"));
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
				bool& bunnyhopref = config.getRef<bool>(vars.bBunnyHop);
				ImGui::Checkbox(XOR("Bunnyhop"), &bunnyhopref);
				if (bunnyhopref)
				{
					ImGui::SliderInt(XOR("Bunnyhop hitchance"), &config.getRef<int>(vars.iBunnyHopChance), 0, 100, "%i%%");
				}
				ImGui::Combo(XOR("Autostrafe"), &config.getRef<int>(vars.iAutoStrafe), selections::straferModes);
				bool& logRef = config.getRef<bool>(vars.bLogEnabled);
				ImGui::Checkbox(XOR("Log events"), &logRef);
				if (logRef)
				{
					ImGui::SliderFloat(XOR("Log time"), &config.getRef<float>(vars.fLogMaxTime), 1.0f, 8.0f);
				}
				ImGui::Checkbox(XOR("Hitmarker"), &config.getRef<bool>(vars.bDrawHitmarker));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("3D##Hitm"), &config.getRef<bool>(vars.bDrawHitmarker3D));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Resize##Hitm"), &config.getRef<bool>(vars.bDrawHitmarkerResize));
				ImGui::ColorPicker(XOR("Hitmarker normal"), &config.getRef<CfgColor>(vars.cDrawHitmarkerNormal));
				ImGui::ColorPicker(XOR("Hitmarker hs"), &config.getRef<CfgColor>(vars.cDrawHitmarkerHead));
				ImGui::SameLine();
				ImGui::Text(XOR("Hitmarker hs"));
				ImGui::ColorPicker(XOR("Hitmarker dead"), &config.getRef<CfgColor>(vars.cDrawHitmarkerDead));
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
					ImGui::Checkbox(XOR("Show always"), &config.getRef<bool>(vars.bNadePredAlways));
					ImGui::SameLine();
					ImGui::ColorPicker(XOR("Nade pred color"), &config.getRef<CfgColor>(vars.cNadePredColor));
					ImGui::ColorPicker(XOR("Nade pred color box fill"), &config.getRef<CfgColor>(vars.cNadeBoxColorFill));
					ImGui::SameLine();
					ImGui::ColorPicker(XOR("Nade pred color box outline"), &config.getRef<CfgColor>(vars.cNadeBoxColorOutline));
				}
				bool& trailref = config.getRef<bool>(vars.bNadeTracer);
				ImGui::Checkbox(XOR("Nade trails"), &trailref);
				if (trailref)
				{
					ImGui::ColorPicker(XOR("Nade trail color"), &config.getRef<CfgColor>(vars.cNadeTracer));
					ImGui::SliderFloat(XOR("Nade trail max dist"), &config.getRef<float>(vars.fNadeTracerMaxDist), 1.0f, 100.0f);
					ImGui::Checkbox(XOR("Nade trail warn"), &config.getRef<bool>(vars.bNadeTracerWarn));
					ImGui::SameLine();
					ImGui::HelpMarker(XOR("Draws scaled circles behind you where nade is"));
				}
				bool& tref = config.getRef<bool>(vars.bRunMovementTrail);
				ImGui::Checkbox(XOR("Movement trail"), &tref);
				if (tref)
				{
					ImGui::Combo(XOR("Trail type"), &config.getRef<int>(vars.iRunMovementTrail), selections::trails);
					ImGui::SameLine();
					ImGui::ColorPicker(XOR("Movement trail color"), &config.getRef<CfgColor>(vars.cMovementTrail));
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
						ImGui::ColorPicker(XOR("Hat triangle color"), &config.getRef<CfgColor>(vars.cHatTriangle));
						ImGui::ColorPicker(XOR("Hat lines color"), &config.getRef<CfgColor>(vars.cHatLine));
					}
					else
					{
						ImGui::SliderFloat(XOR("Hat rainbow speed"), &config.getRef<float>(vars.fHatSpeed), 0.2f, 10.0f);
					}
					ImGui::SliderFloat(XOR("Hat size"), &config.getRef<float>(vars.fHatSize), -100.0f, 100.0f);
					ImGui::SliderFloat(XOR("Hat radius"), &config.getRef<float>(vars.fHatRadius), 1.0f, 100.0f);
				}
				//ImGui::Checkbox(XOR("Remove blood spray"), &config.getRef<bool>(vars.bRemoveBloodSpray));
				ImGui::Checkbox(XOR("Enable tracers"), &config.getRef<bool>(vars.bDrawBulletTracer));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Tracers color"), &config.getRef<CfgColor>(vars.cDrawBulletTracer));
				ImGui::SliderFloat(XOR("Tracers life"), &config.getRef<float>(vars.fDrawBulletTracer), 0.0f, 5.0f);
				ImGui::Checkbox(XOR("Enable local bullets"), &config.getRef<bool>(vars.bDrawLocalSideImpacts));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Local bullets line color"), &config.getRef<CfgColor>(vars.cDrawLocalSideImpactsLine));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Local bullets fill color"), &config.getRef<CfgColor>(vars.cDrawLocalSideImpactsFill));
				ImGui::SliderFloat(XOR("Local bullets time"), &config.getRef<float>(vars.fDrawLocalSideImpacts), 0.0f, 5.0f);
				ImGui::Checkbox(XOR("Enable client bullets"), &config.getRef<bool>(vars.bDrawClientSideImpacts));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Client bullets line color"), &config.getRef<CfgColor>(vars.cDrawClientSideImpactsLine));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Client bullets fill color"), &config.getRef<CfgColor>(vars.cDrawClientSideImpactsFill));
				ImGui::SliderFloat(XOR("Client bullets time"), &config.getRef<float>(vars.fDrawClientSideImpacts), 0.0f, 5.0f);
				ImGui::Checkbox(XOR("Freelook"), &config.getRef<bool>(vars.bFreeLook));
				ImGui::SameLine();
				ImGui::Hotkey(XOR("##fl"), &config.getRef<Key>(vars.kFreeLook));
				ImGui::Checkbox(XOR("FreeCam"), &config.getRef<bool>(vars.bFreeCam));
				ImGui::SameLine();
				ImGui::Hotkey("##fc", &config.getRef<Key>(vars.kFreeCam));
				ImGui::SliderFloat(XOR("Speed##fc"), &config.getRef<float>(vars.fFreeCam), 1.0f, 20.0f);
				ImGui::Checkbox(XOR("MirrorCam"), &config.getRef<bool>(vars.bMirrorCam));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("On key##mcam"), &config.getRef<bool>(vars.bMirrorCamOnKey));
				ImGui::SameLine();
				ImGui::Hotkey(XOR("##mcam"), &config.getRef<Key>(vars.kMirrorCam));
				ImGui::Checkbox(XOR("Flashlight"), &config.getRef<bool>(vars.bFlashlight));
				ImGui::SameLine();
				ImGui::Checkbox(XOR("Big mode"), &config.getRef<bool>(vars.bFlashlightBigMode));
				ImGui::SameLine();
				ImGui::Hotkey(XOR("key##flashl"), &config.getRef<Key>(vars.kFlashlight));
				ImGui::SliderFloat(XOR("Flashlight FOV"), &config.getRef<float>(vars.fFlashlightFov), 1.0f, 100.0f);
				ImGui::Checkbox(XOR("Fog enabled"), &config.getRef<bool>(vars.bFog));
				ImGui::SliderFloat(XOR("Fog distance"), &config.getRef<float>(vars.fFogDistance), 1.0f, 1000.0f);
				ImGui::ColorPicker(XOR("Fog color"), &config.getRef<CfgColor>(vars.cFog));
				ImGui::Combo(XOR("Screen effect"), &config.getRef<int>(vars.iScreenEffect), selections::screenEffects);
				ImGui::SliderFloat(XOR("Param##Screen effect"), &config.getRef<float>(vars.fScreenEffectParam), 0.0f, 1.0f);
				ImGui::ColorPicker(XOR("Color##Screen effect"), &config.getRef<CfgColor>(vars.cScreenEffect));
				bool changedbut = false;
				changedbut |= ImGui::Checkbox(XOR("ControlTone enabled"), &config.getRef<bool>(vars.bControlTone));
				world.setCheckStateButton(changedbut);
				bool changed1 = false;
				changed1|= ImGui::SliderFloat(XOR("Tone min"), &config.getRef<float>(vars.fControlToneMin), 0.0f, 1.0f);
				bool changed2 = false;
				changed2 |= ImGui::SliderFloat(XOR("Tone max"), &config.getRef<float>(vars.fControlToneMax), 0.0f, 1.0f);
				world.setCheckStateSlider(changed1 || changed2);
				ImGui::Checkbox(XOR("Weather"), &config.getRef<bool>(vars.bWeather));
				world.implMenu();
				ImGui::Checkbox(XOR("Motion blur"), &config.getRef<bool>(vars.bMotionBlur));
				ImGui::Checkbox(XOR("Forward##Motion Blur"), &config.getRef<bool>(vars.bMotionBlurForward));
				ImGui::SliderFloat(XOR("Failling intensity##Motion Blur"), &config.getRef<float>(vars.fMotionBlurFallingIntensity), 0.0f, 5.0f);
				ImGui::SliderFloat(XOR("Falling max##Motion Blur"), &config.getRef<float>(vars.fMotionBlurFallingMax), 0.0f, 30.0f);
				ImGui::SliderFloat(XOR("Falling min##Motion Blur"), &config.getRef<float>(vars.fMotionBlurFallingMin), 0.0f, 30.0f);
				ImGui::SliderFloat(XOR("Strength##Motion Blur"), &config.getRef<float>(vars.fMotionBlurGlobalStrength), 0.0f, 20.0f);
				ImGui::SliderFloat(XOR("Roll intensity##Motion Blur"), &config.getRef<float>(vars.fMotionBlurRollIntensity), 0.0f, 1.0f);
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

	static auto initIndexOnce = []()
	{
		for (size_t i = 0; const auto & el : config.getAllConfigFiles())
		{
			if (el == config.getCfgToLoad())
				currentcfg = i;

			i++;
		}

		return true;
	} ();

	ImGui::Columns(2, nullptr, false);
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
					// this is only saving the load name, nothing more
					config.saveCfgToLoad(allcfg.at(currentcfg));
					console.log(TypeLogs::LOG_INFO, XOR("{} will be now loaded config on the start"), allcfg.at(currentcfg));
				}
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("This config will load on the start\nThis button saves default config!"));
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	if (ImGui::BeginChild(XOR("cfgkeys"), {}, true, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::BeginGroupPanel(XOR("Config keys"));
		{
			ImGui::Hotkey(XOR("Menu key"), &config.getRef<Key>(vars.kMenu), false);
			ImGui::Hotkey(XOR("Console key"), &config.getRef<Key>(vars.kConsoleLog), false);
			ImGui::Hotkey(XOR("Panic key"), &config.getRef<Key>(vars.kPanic), false);
			ImGui::Checkbox(XOR("Enable x88 menu"), &config.getRef<bool>(vars.bMenuOpenedx88));
		}
		ImGui::EndGroupPanel();

		ImGui::EndChild();
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

#include "background.hpp"

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
				ImGui::Checkbox(XOR("Background"), &config.getRef<bool>(vars.bBackround));
				ImGui::SliderFloat(XOR("Background speed"), &config.getRef<float>(vars.fBackground), 1.0f, 8.0f);
				ImGui::SliderFloat(XOR("Background max dist"), &config.getRef<float>(vars.fBackgroundDistance), 10.0f, 400.0f);
				ImGui::SliderInt(XOR("Background amount of records"), &config.getRef<int>(vars.iBackgroundSize), 20, 400);
				ImGui::Text(XOR("They are temp limited with dynamic size + saving!"));
				ImGui::ColorPicker(XOR("Background color1"), &config.getRef<CfgColor>(vars.cBackGround1));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Background color2"), &config.getRef<CfgColor>(vars.cBackGround2));
				ImGui::SameLine();
				ImGui::ColorPicker(XOR("Background color3"), &config.getRef<CfgColor>(vars.cBackGround3));
				if (ImGui::Button(XOR("Refresh background")))
					background.init();
				ImGui::Checkbox(XOR("Discord RPC"), &config.getRef<bool>(vars.bDiscord));
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
	if (m_active)
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