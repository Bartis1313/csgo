#include "menu.hpp"

#include "styles.hpp"
#include "animations.hpp"

#include <dependencies/ImGui/imgui.h>
#include <dependencies/ImGui/imgui_impl_win32.h>
#include <dependencies/ImGui/imgui_impl_dx9.h>
#include <dependencies/ImGui/imgui_internal.h>
#include <dependencies/ImGui/imgui_stdlib.h>
#include <dependencies/magic_enum.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/console/console.hpp>
#include <config/vars.hpp>

// see the code of dear imgui lib or demo window to change it.
void ImGuiMenu::init()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	style = vars::styling->imStyle;

	// NEED static here
	static auto p = (config.getHackPath() / XOR("window.ini")).string();

	io.IniFilename = p.c_str();
	io.LogFilename = nullptr;

	try
	{
		imRender.init(io);
	}
	catch (const std::runtime_error& err)
	{
		LOG_ERR(err.what());
	}
}

void ImGuiMenu::shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
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
#include "selections.hpp"

static ImVec2 availRegion()
{
	// we could manually count the difference between columns
	ImVec2 avail = ImGui::GetContentRegionAvail();
	return avail;
}

static ImVec2 getChildSize(size_t numOfChilds = 1) // num of childs in single column
{
	// x is always corresponding to avail region
	float x = ImGui::GetContentRegionAvail().x;
	return ImVec2{ x, ImGui::GetContentRegionAvail().y / numOfChilds };
}

static ImVec2 getChildSizeCustom(float y)
{
	// x is always corresponding to avail region
	float x = ImGui::GetContentRegionAvail().x;
	return ImVec2{ x, y };
}

static void renderAimbot()
{
	// using columns, it will be easier, imgui by default recommends begintable(), but in this case columns do stuff like padding each column for us.
	static int index = 0;

	auto& vec = vars::aim->weapons;
	auto& cfg = vec.at(index);

	ImGui::Columns(2, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("aim main"), {}, true))
		{
			ImGui::BeginGroupPanel(XOR("Aimbot"), availRegion());
			{
				ImGui::Animations::ListBox<const std::string>(XOR("Weapon selection"), &index, magic_enum::enum_names_pretty<AimbotWeapon>());
				ImGui::Animations::Checkbox(XOR("Enabled##aim"), &cfg.enabled);
				ImGui::SameLine();
				ImGui::Animations::Checkbox(XOR("Use key##aimbot"), &vars::aim->useKey);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("", &vars::keys->aimbot);
				ImGui::Animations::SliderFloat(XOR("Fov##aim"), &cfg.fov, 0.0f, 50.0f);
				ImGui::Animations::Combo<const std::string>(XOR("Method##Aim"), &cfg.methodAim, magic_enum::enum_names_pretty<AimbotMethod>());
				ImGui::Animations::Combo<const std::string>(XOR("Hitboxes##aim"), &cfg.aimSelection, magic_enum::enum_names_pretty<AimbotHitboxes>());
				ImGui::Animations::SliderFloat(XOR("Smooth##aim"), &cfg.smooth, 0.0f, 1.0f);
				ImGui::Animations::Combo<const std::string>(XOR("Smooth method##aim"), &cfg.smoothMode, magic_enum::enum_names_pretty<SmoothMode>());
				/*if (cfg.smoothMode == E2T(SmoothMode::HUMANIZED))
				{
					ImGui::Animations::Checkbox(XOR("Extra speed#aim"), &cfg.useExtraSpeed);
					ImGui::SameLine();
					ImGui::HelpMarker(XOR("Works good on high smoothing values"));
				}*/
				ImGui::Animations::Checkbox(XOR("Curved##aim"), &cfg.curveAim);
				ImGui::Animations::SliderFloat(XOR("Curve X##aim"), &cfg.curveX, 0.0f, 1.0f);
				ImGui::Animations::SliderFloat(XOR("Curve Y##aim"), &cfg.curveY, 0.0f, 1.0f);
				ImGui::Animations::Checkbox(XOR("Delay##aim"), &cfg.aimDelay);
				ImGui::Animations::SliderFloat(XOR("Delay ms##aim"), &cfg.aimDelayVal, 0.0f, 800.0f);
				ImGui::Animations::Checkbox(XOR("Aim at Backtrack"), &cfg.aimBacktrack);
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Will aim at middle of records!"));
				ImGui::Animations::Checkbox(XOR("Smoke check##Aimbot"), &cfg.smokeCheck);
				ImGui::Animations::SliderFloat(XOR("Flash limit##Aimbot"), &cfg.flashLimit, 0.0f, 255.0f);

				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}

	ImGui::NextColumn();

	if (ImGui::BeginChild(XOR("aim extra"), {}, true), ImGuiWindowFlags_AlwaysAutoResize)
	{
		ImGui::BeginGroupPanel(XOR("Backtracking & Latency"), availRegion());
		{
			ImGui::Animations::Checkbox(XOR("Backtrack##enabled"), &vars::backtrack->enabled);
			ImGui::Animations::SliderFloat(XOR("Backtrack ms"), &vars::backtrack->time, 0.0f, 200.0f);
			ImGui::Animations::Checkbox(XOR("Smoke check##Backtrack"), &vars::backtrack->smoke);
			ImGui::Animations::SliderFloat(XOR("Flash limit##Backtrack"), &vars::backtrack->flashLimit, 0.0f, 255.0f);
			ImGui::Animations::Checkbox(XOR("Latency##enabled"), &vars::misc->fakeLatency->enabled);
			ImGui::Animations::SliderFloat(XOR("Fake latency ms"), &vars::misc->fakeLatency->amount, 0.0f, 200.0f);

			ImGui::EndGroupPanel();
		}

		ImGui::BeginGroupPanel(XOR("Triggerbot"), availRegion());
		{
			ImGui::Animations::Checkbox(XOR("Triggerbot enabled"), &cfg.triggerbot);
			ImGui::Animations::SliderFloat(XOR("Triggerbot ms"), &cfg.triggerbotDelay, 0.0f, 200.0f);

			ImGui::EndGroupPanel();
		}

		ImGui::BeginGroupPanel(XOR("RCS"), availRegion());
		{
			ImGui::Animations::Checkbox(XOR("Enabled##RCS"), &cfg.rcs);
			ImGui::Animations::SliderFloat(XOR("X%##Rcsx"), &cfg.rcsX, 0.0f, 1.0f);
			ImGui::Animations::SliderFloat(XOR("Y%##Rcsy"), &cfg.rcsY, 0.0f, 1.0f);

			ImGui::EndGroupPanel();
		}

		ImGui::BeginGroupPanel(XOR("Drawing"), availRegion());
		{
			ImGui::Animations::Checkbox(XOR("Draw fov"), &vars::aimPaint->enabledFov);
			ImGui::SameLine();
			ImGui::Animations::ColorPicker(XOR("Fov circle color"), &vars::aimPaint->colorFov);
			ImGui::SameLine();
			ImGui::HelpMarker(XOR("Draws a circle representing your aimbot FOV"));
			ImGui::Animations::Checkbox(XOR("Draw aimbot point"), &vars::aimPaint->enabledPoint);
			ImGui::SameLine();
			ImGui::Animations::ColorPicker(XOR("Color##bestpoint"), &vars::aimPaint->colorPoint);

			ImGui::EndGroupPanel();
		}
	}
	ImGui::EndChild();

	ImGui::Columns();
}

static void renderVisuals()
{
	ImGui::Columns(2, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("visuals"), {}, true))
		{
			ImGui::BeginGroupPanel(XOR("Players"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Enabled"), &vars::visuals->esp->boxes->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Player boxes"), []()
					{
						ImGui::Animations::Combo<const std::string>(XOR("Boxes mode"), &vars::visuals->esp->boxes->mode, magic_enum::enum_names_pretty<BoxTypes>(), 40);
						ImGui::Animations::ColorPicker(XOR("color##box"), &vars::visuals->esp->boxes->color);
						ImGui::Animations::ColorPicker(XOR("filled##box"), &vars::visuals->esp->boxes->fill);
						ImGui::Animations::Checkbox(XOR("Outlined"), &vars::visuals->esp->boxes->outline);
						if (auto type = vars::visuals->esp->boxes->mode; type == E2T(BoxTypes::FILLED2D) || type == E2T(BoxTypes::FILLED3D))
						{
							ImGui::Animations::Checkbox(XOR("Multi color rect fill"), &vars::visuals->esp->boxes->multiColor);
							ImGui::Animations::SliderFloat(XOR("Rect fill multigradient speed"), &vars::visuals->esp->boxes->multiColorSpeed, 0.0f, 15.0f);
						}
					}
				);

				ImGui::Animations::Checkbox(XOR("Health"), &vars::visuals->esp->healthBar->enabled);
				ImGui::Animations::Checkbox(XOR("Armor"), &vars::visuals->esp->armorBar->enabled);
				ImGui::Animations::Checkbox(XOR("Name"), &vars::visuals->esp->nameBar->enabled);
				ImGui::Animations::Checkbox(XOR("Enemy aiming warn"), &vars::misc->aimWarn->enabled);
				ImGui::Animations::Checkbox(XOR("Skeleton"), &vars::visuals->esp->skeleton->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Skeleton ESP options"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Skeleton color"), &vars::visuals->esp->skeleton->color);
						ImGui::Animations::Checkbox(XOR("Draw debug points##skelet"), &vars::visuals->esp->skeleton->showDebug);
					}
				);

				ImGui::Animations::Checkbox(XOR("Sound ESP"), &vars::visuals->sound->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Sound ESP options"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Circles color"), &vars::visuals->sound->color);
						ImGui::Animations::SliderFloat(XOR("Step time"), &vars::visuals->sound->time, 1.0f, 20.0f);
						ImGui::Animations::SliderFloat(XOR("Step max distance"), &vars::visuals->sound->maxDist, 5.0f, 500.0f);
						ImGui::Animations::SliderFloat(XOR("Lines info distance"), &vars::visuals->sound->maxDistLine, 2.0f, 200.0f);
						ImGui::HelpMarker(XOR("Max pixels to decide to draw info from centre of screen to the best point"));
						ImGui::Animations::ColorPicker(XOR("Lines info color"), &vars::visuals->sound->colorLine);
					}
				);
				ImGui::Animations::Checkbox(XOR("Dlight"), &vars::visuals->esp->dlight->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Dlight options"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Dlight color"), &vars::visuals->esp->dlight->color);
						ImGui::Animations::SliderFloat(XOR("Dlight radius"), &vars::visuals->esp->dlight->radius, 0.0f, 300.0f);
						ImGui::Animations::SliderFloat(XOR("Dlight exponent"), &vars::visuals->esp->dlight->exponent, 0.0f, 12.0f);
						ImGui::Animations::SliderFloat(XOR("Dlight decay"), &vars::visuals->esp->dlight->decay, 0.0f, 100.0f);
					}
				);
				ImGui::Animations::MultiCombo(XOR("Esp flags"), magic_enum::enum_names_pretty<EspFlags>(), &vars::visuals->esp->flags->flags);
				ImGui::Animations::Checkbox(XOR("Visible only##Visuals"), &vars::visuals->esp->checks->visible);
				ImGui::Animations::Checkbox(XOR("Dead only ##Visuals"), &vars::visuals->esp->checks->dead);
				ImGui::Animations::Checkbox(XOR("Smoke check##Visuals"), &vars::visuals->esp->checks->smoke);
				ImGui::Animations::SliderFloat(XOR("Flash limit##Visuals"), &vars::visuals->esp->checks->flashLimit, 0.0f, 255.0f);
				ImGui::Animations::SliderFloat(XOR("Dormacy time##Visuals"), &vars::visuals->dormacy->time, 0.0f, 15.0f);
				ImGui::Animations::SliderFloat(XOR("Dormacy time box##Visuals"), &vars::visuals->dormacy->limit, 0.0f, 15.0f);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Chams"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Chams enabled Players"), &vars::visuals->chams->players);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Chams options players"), []()
					{
						ImGui::Animations::Combo<const std::string>(XOR("Chams type Players"), &vars::visuals->chams->indexPlayers, magic_enum::enum_names_pretty<ChamsType>());
						ImGui::Animations::ColorPicker(XOR("Chams color Players"), &vars::visuals->chams->colorPlayers);
						ImGui::Animations::Checkbox(XOR("Chams XQZ"), &vars::visuals->chams->enabledXQZPlayers);
						ImGui::Animations::ColorPicker(XOR("Chams XQZ color"), &vars::visuals->chams->colorXQZPlayers);
					});
				ImGui::Animations::Checkbox(XOR("Chams enabled Weapons"), &vars::visuals->chams->enabledWeapons);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Chams options weapons"), []()
					{
						ImGui::Animations::Combo<const std::string>(XOR("Chams type Weapons"), &vars::visuals->chams->indexWeapons, magic_enum::enum_names_pretty<ChamsType>());
						ImGui::Animations::ColorPicker(XOR("Chams color Weapons"), &vars::visuals->chams->colorWeapons);
						ImGui::Animations::Checkbox(XOR("Disable weapon"), &vars::visuals->chams->weaponHide);
					});
				ImGui::Animations::Checkbox(XOR("Chams enabled Arms"), &vars::visuals->chams->enabledArms);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Chams options arms"), []()
					{
						ImGui::Animations::Combo<const std::string>(XOR("Chams type Arms"), &vars::visuals->chams->indexArms, magic_enum::enum_names_pretty<ChamsType>());
						ImGui::Animations::ColorPicker(XOR("Chams color Arms"), &vars::visuals->chams->colorArms);
						ImGui::Animations::Checkbox(XOR("Disable arms"), &vars::visuals->chams->armsHide);
					});
				ImGui::Animations::Checkbox(XOR("Chams enabled Backtrack"), &vars::visuals->chams->enabledBacktrack);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Chams options backtrack"), []()
					{
						ImGui::Animations::Combo<const std::string>(XOR("Chams type Backtrack"), &vars::visuals->chams->modeBacktrack, magic_enum::enum_names_pretty<ChamsType>());
						ImGui::Animations::ColorPicker(XOR("Backtrack chams col"), &vars::visuals->chams->colorBacktrack);
						ImGui::Animations::Combo<const std::string>(XOR("Chams style Backtrack"), &vars::visuals->chams->indexBacktrack, magic_enum::enum_names_pretty<BTChamsType>());
					});
				ImGui::Animations::Checkbox(XOR("Enabled glow"), &vars::visuals->glow->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Chams options"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Glow color"), &vars::visuals->glow->colorPlayer);
					});

				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		if (ImGui::BeginChild("wpns", {}, true))
		{
			ImGui::BeginGroupPanel(XOR("Weapons"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Enabled##weapons"), &vars::visuals->esp->weaponBar->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Weapons text and bar"), []()
					{
						ImGui::Animations::Checkbox(XOR("Translate name"), &vars::visuals->esp->weaponBar->translate);
						ImGui::Animations::ColorPicker(XOR("Color for text"), &vars::visuals->esp->weaponBar->text);
						ImGui::Animations::ColorPicker(XOR("Reload bar color"), &vars::visuals->esp->weaponBar->bar);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Bomb"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Bomb info"), &vars::visuals->world->bomb->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Bomb info draw"), []()
					{
						ImGui::Animations::ColorPicker(XOR("C4 info"), &vars::visuals->world->bomb->background);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Nades"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Enabled##nades"), &vars::visuals->world->projectiles->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Nades drawing"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Flashbang color"), &vars::visuals->world->projectiles->flash);
						ImGui::Animations::ColorPicker(XOR("Granede color"), &vars::visuals->world->projectiles->nade);
						ImGui::Animations::ColorPicker(XOR("Molotov color"), &vars::visuals->world->projectiles->molotov);
						ImGui::Animations::ColorPicker(XOR("Smoke color"), &vars::visuals->world->projectiles->smoke);
						ImGui::Animations::ColorPicker(XOR("Decoy color"), &vars::visuals->world->projectiles->decoy);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Dropped"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Enabled##dropped"), &vars::visuals->esp->dropped->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Dropped weapons draw"), []()
					{
						ImGui::Animations::MultiCombo(XOR("Dropped flags"), magic_enum::enum_names_pretty<DroppedFlags>(), &vars::visuals->esp->dropped->flags);
						ImGui::Animations::ColorPicker(XOR("Dropped color"), &vars::visuals->esp->dropped->color);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Particles edit"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("##Edit molotov"), &vars::visuals->world->particles->enabledMolotov);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker(XOR("Molotov##edited"), &vars::visuals->world->particles->colorMolotov);
				ImGui::Animations::Checkbox(XOR("##Edit blood"), &vars::visuals->world->particles->enabledBlood);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker(XOR("Blood##edited"), &vars::visuals->world->particles->colorBlood);
				ImGui::Animations::Checkbox(XOR("##Edit smoke"), &vars::visuals->world->particles->enabledSmoke);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker(XOR("Smoke##edited"), &vars::visuals->world->particles->colorSmoke);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Molotov & Smoke circles"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("##Enabled molotov circle"), &vars::visuals->world->molotov->enabled);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker(XOR("Draw molotov circle"), &vars::visuals->world->molotov->color);
				ImGui::Animations::Checkbox(XOR("##Enabled smoke circle"), &vars::visuals->world->smoke->enabled);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker(XOR("Draw smoke circle"), &vars::visuals->world->smoke->color);

				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}
	ImGui::Columns();
}

#include <features/visuals/radar/radar.hpp>
#include <features/visuals/world/skybox.hpp>
#include <features/visuals/world/weather.hpp>
#include <features/visuals/world/ambient.hpp>
#include <features/visuals/world/tone.hpp>

static void renderMisc()
{
	ImGui::Columns(2, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("Misc"), {}, true))
		{
			ImGui::BeginGroupPanel(XOR("Skybox"), availRegion());
			{
				const auto customsky = g_SkyboxEdit->getAllCustomSkyBoxes();
				bool state;
				state |= ImGui::Animations::Combo<const char*>(XOR("Normal"), &vars::visuals->world->sky->indexNormal, selections::skyboxes);
				state |= ImGui::Animations::Combo<const std::string>(XOR("Custom##Skybox"), &vars::visuals->world->sky->indexCustom, customsky);
				g_SkyboxEdit->handleButtonState(state);
				if (ImGui::Animations::Button(XOR("Reload Custom Skybox")))
				{
					g_SkyboxEdit->reloadCustomSkyboxes();
				}

				ImGui::EndGroupPanel();
			}
			ImGui::BeginGroupPanel(XOR("World Modulation"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Modulate world"), &vars::visuals->world->modulate->enabled);
				ImGui::Animations::Checkbox(XOR("Remove sky"), &vars::visuals->world->sky->removeSky);
				ImGui::Animations::ColorPicker(XOR("Modulate world texture"), &vars::visuals->world->modulate->texture);
				ImGui::Animations::ColorPicker(XOR("Modulate world prop"), &vars::visuals->world->modulate->prop);
				ImGui::Animations::ColorPicker(XOR("Modulate world sky"), &vars::visuals->world->modulate->sky);
				ImGui::Animations::SliderFloat(XOR("Shader value"), &vars::visuals->world->modulate->shader, 0.0f, 100.0f);

				ImGui::EndGroupPanel();
			}


			/*ImGui::Animations::Checkbox(XOR("Zeus range"), &config.getRef<bool>(vars.bDrawZeusRange));
			ImGui::SameLine();
			ImGui::Animations::ColorPicker(XOR("Zeus color"), &config.getRef<CfgColor>(vars.cZeusRange));
			ImGui::Animations::Checkbox(XOR("Zeus party"), &config.getRef<bool>(vars.bZeusPartyMode));
			ImGui::SameLine();
			ImGui::Animations::Checkbox(XOR("Zeus tracing"), &config.getRef<bool>(vars.bZeusUseTracing));*/

			ImGui::BeginGroupPanel(XOR("View"), availRegion());
			{
				ImGui::Animations::SliderFloat(XOR("FOV local"), &vars::misc->fov->value, -50.0f, 50.0f);
				ImGui::Animations::Checkbox(XOR("Third Person"), &vars::misc->thirdp->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey(XOR(""), &vars::keys->thirdP);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Third person pop"), []()
					{
						ImGui::Animations::SliderFloat(XOR("Distance##thirdp"), &vars::misc->thirdp->distance, 1.0f, 500.0f);
						ImGui::Animations::SliderFloat(XOR("Extra X##thirdp"), &vars::misc->thirdp->x, -180.0f, 180.0f);
						ImGui::Animations::SliderFloat(XOR("Extra Y##thirdp"), &vars::misc->thirdp->y, -180.0f, 180.0f);
					}
				);
				ImGui::Animations::Combo<const std::string>(XOR("Crosshair type"), &vars::misc->crosshair->index, magic_enum::enum_names_pretty<CrossHairTypes>());

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Radar"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("2D Radar enabled"), &vars::misc->radar->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##2D Radar pop"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Angle view line"), &vars::misc->radar->colorLine);
						ImGui::Animations::ColorPicker(XOR("Players color"), &vars::misc->radar->colorPlayer);
						ImGui::Animations::SliderFloat(XOR("Thickness"), &vars::misc->radar->thickness, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat(XOR("Length"), &vars::misc->radar->length, 0.0f, 40.0f);
						ImGui::Animations::SliderFloat(XOR("Scale"), &vars::misc->radar->scale, 0.0f, 10.0f);
						ImGui::Animations::SliderFloat(XOR("Radar size"), &vars::misc->radar->size, 10.0f, 500.0f);
						ImGui::Animations::Checkbox(XOR("Draw out of radar"), &vars::misc->radar->ranges);
						ImGui::SameLine();
						ImGui::HelpMarker(XOR("If enemy is out of the radar\nThen icons will still appear but clamped"));
					}
				);
				if (ImGui::Animations::Button(XOR("Refresh texture manually")))
				{
					g_Radar->manuallyInitTexture();
				}
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Will not for workshop maps\nYou can try forcing the engine to re-render by pressing escape few times"));
			}
			ImGui::EndGroupPanel();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		if (ImGui::BeginChild(XOR("Miscother"), {}, true))
		{
			ImGui::BeginGroupPanel(XOR("Movement"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Bunnyhop"), &vars::misc->bunnyHop->enabled);
				if(vars::misc->bunnyHop->enabled)
				{
					ImGui::SliderInt(XOR("Bunnyhop hitchance"), &vars::misc->bunnyHop->chance, 0, 100, "%i%%");
				}
				ImGui::Animations::Combo<const std::string>(XOR("Autostrafe"), &vars::misc->bunnyHop->indexStrafe, magic_enum::enum_names_pretty<MovementStraferMode>());

				ImGui::EndGroupPanel();
			}
			/*bool& logRef = config.getRef<bool>(vars.bLogEnabled);
			ImGui::Animations::Checkbox(XOR("Log events"), &logRef);
			if (logRef)
			{
				ImGui::Animations::SliderFloat(XOR("Log time"), &config.getRef<float>(vars.fLogMaxTime), 1.0f, 8.0f);
			}*/
			ImGui::BeginGroupPanel(XOR("Hitmarker"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Hitmarker"), &vars::misc->hitmarker->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##hitmarker pop"), []()
					{
						ImGui::Animations::Checkbox(XOR("3D##Hitm"), &vars::misc->hitmarker->enabled3D);
						ImGui::SameLine();
						ImGui::Animations::Checkbox(XOR("Resize##Hitm"), &vars::misc->hitmarker->enabledResize);
						ImGui::Animations::ColorPicker(XOR("Hitmarker normal"), &vars::misc->hitmarker->colorNormal);
						ImGui::Animations::ColorPicker(XOR("Hitmarker hs"), &vars::misc->hitmarker->colorHead);
						ImGui::SameLine();
						ImGui::Text(XOR("Hitmarker hs"));
						ImGui::Animations::ColorPicker(XOR("Hitmarker dead"), &vars::misc->hitmarker->colorDead);
						ImGui::Animations::SliderFloat(XOR("Hitmarker time"), &vars::misc->hitmarker->time, 0.0f, 2.0f);
						ImGui::Animations::Checkbox(XOR("Play hitmarker"), &vars::misc->hitmarker->play);
						ImGui::SameLine();
						ImGui::HelpMarker(XOR("Will play the sound after every kill"));
					}
				);

				ImGui::EndGroupPanel();
			}
			ImGui::BeginGroupPanel(XOR("Misc"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("No scope"), &vars::misc->scope->enabled);
				
				ImGui::Animations::Checkbox(XOR("FPS Plot"), &vars::misc->plots->enabledFps);
				if (vars::misc->plots->enabledFps)
				{
					ImGui::SameLine();
					ImGui::Animations::Checkbox(XOR("FPS Custom"), &vars::misc->plots->fpsCustom);
				}
				
				ImGui::Animations::Checkbox(XOR("Velocity Plot"), &vars::misc->plots->enabledVelocity);
				if (vars::misc->plots->enabledVelocity)
				{
					ImGui::SameLine();
					ImGui::Animations::Checkbox(XOR("Velocity Custom"), &vars::misc->plots->velocityCustom);
				}
				ImGui::Animations::Checkbox(XOR("Draw misc info"), &vars::misc->info->enabled);
				ImGui::Animations::Checkbox(XOR("Playerlist##checkbox"), &vars::misc->playerList->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Playerlist pop"), []()
					{
						ImGui::Animations::Checkbox(XOR("Health##playerlist"), &vars::misc->playerList->health);
						ImGui::Animations::Checkbox(XOR("Money##playerlist"), &vars::misc->playerList->money);
						ImGui::Animations::Checkbox(XOR("Team##playerlist"), &vars::misc->playerList->teamID);
						ImGui::Animations::Checkbox(XOR("Place##playerlist"), &vars::misc->playerList->lastPlace);
					}
				);
				ImGui::Animations::Checkbox(XOR("Hat on local"), &vars::misc->hat->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Hat pop"), []()
					{
						ImGui::Animations::Checkbox(XOR("Rainbow hat"), &vars::misc->hat->rainbow);
						if (!vars::misc->hat->rainbow)
						{
							ImGui::Animations::ColorPicker(XOR("Hat triangle color"), &vars::misc->hat->colorTriangle);
							ImGui::Animations::ColorPicker(XOR("Hat lines color"), &vars::misc->hat->colorLine);
						}
						else
						{
							ImGui::Animations::SliderFloat(XOR("Hat rainbow speed"), &vars::misc->hat->rainbowSpeed, 0.2f, 10.0f);
						}
						ImGui::Animations::SliderFloat(XOR("Hat size"), &vars::misc->hat->size, -100.0f, 100.0f);
						ImGui::Animations::SliderFloat(XOR("Hat radius"), &vars::misc->hat->radius, 1.0f, 100.0f);
					}
				);

				ImGui::EndGroupPanel();
			}
			ImGui::BeginGroupPanel(XOR("Nade prediction"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Nade pred"), &vars::misc->nade->enabledPred);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##nade pred pop"), []()
					{
						ImGui::Animations::Checkbox(XOR("Show always"), &vars::misc->nade->predAlways);
						ImGui::Animations::ColorPicker(XOR("Nade pred color"), &vars::misc->nade->colorPredLine);
						ImGui::Animations::ColorPicker(XOR("Nade pred color box fill"), &vars::misc->nade->colorPredBoxFill);
						ImGui::Animations::ColorPicker(XOR("Nade pred color box outline"), &vars::misc->nade->colorPredBox);
					}
				);
				ImGui::Animations::Checkbox(XOR("Nade trails"), &vars::misc->nade->enabledTracer);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##nade trails pop"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Nade trail color"), &vars::misc->nade->colorTracer);
						ImGui::Animations::SliderFloat(XOR("Nade trail max dist"), &vars::misc->nade->tracerDist, 1.0f, 100.0f);
						ImGui::Animations::Checkbox(XOR("Nade trail warn"), &vars::misc->nade->tracerWarn);
						ImGui::SameLine();
						ImGui::HelpMarker(XOR("Draws scaled circles behind you where nade is"));
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Bullets and trails"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Movement trail"), &vars::misc->trail->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Movement trails pop"), []()
					{
						ImGui::Animations::Combo<const std::string>(XOR("Trail type"), &vars::misc->trail->mode, magic_enum::enum_names_pretty<MovementTrail>());
						ImGui::Animations::ColorPicker(XOR("Movement trail color"), &vars::misc->trail->color);
						ImGui::Animations::SliderFloat(XOR("Trail speed"), &vars::misc->trail->beamSpeed, 1.0f, 10.0f);
						ImGui::Animations::SliderFloat(XOR("Trail life"), &vars::misc->trail->time, 1.0f, 10.0f);
					}
				);


				//ImGui::Animations::Checkbox(XOR("Remove blood spray"), &config.getRef<bool>(vars.bRemoveBloodSpray));
				ImGui::Animations::Checkbox(XOR("Enable tracers"), &vars::visuals->world->tracer->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Bullet tracers"), []()
					{
						ImGui::Animations::Combo<const char*>(XOR("Beam sprite"), &vars::visuals->world->tracer->beamTracer.index, selections::beamNames);
						/*ImGui::InputTextWithHint(XOR("Beam type"), XOR("Add without spaces! eg: 4|8"), &config.getRef<std::string>(vars.sBulletTracerType));*/
						/*ImGui::SameLine();
						ImGui::HelpMarker(XOR("Types list\n"
							"TE_BEAMPOINTS 0\n"
							"TE_SPRITE 1\n"
							"TE_BEAMDISK 2\n"
							"TE_BEAMCYLINDER 3\n"
							"TE_BEAMFOLLOW 4\n"
							"TE_BEAMRING 5\n"
							"TE_BEAMSPLINE 6\n"
							"TE_BEAMRINGPOINT 7\n"
							"TE_BEAMLASER 8\n"
							"TE_BEAMTESLA 9\n"
						));*/
						ImGui::InputTextWithHint(XOR("Beam flags"), XOR("eg: 4|8, spaces allowed"), &vars::visuals->world->tracer->beamTracer.flags);
						ImGui::SameLine();
						ImGui::HelpMarker(XOR("Flags list\n"
							"FBEAM_STARTENTITY 1\n"
							"FBEAM_ENDENTITY 2\n"
							"FBEAM_FADEIN 4\n"
							"FBEAM_FADEOUT 8\n"
							"FBEAM_SINENOISE 16\n"
							"FBEAM_SOLID 32\n"
							"FBEAM_SHADEIN 64\n"
							"FBEAM_SHADEOUT 128\n"
							"FBEAM_ONLYNOISEONCE 256\n"
							"FBEAM_NOTILE 512\n"
							"FBEAM_USE_HITBOXES 1024\n"
							"FBEAM_STARTVISIBLE 2048\n"
							"FBEAM_ENDVISIBLE 4096\n"
							"FBEAM_ISACTIVE 8192\n"
							"FBEAM_FOREVER 16384\n"
							"FBEAM_HALOBEAM 32768\n"
							"FBEAM_REVERSED 65536\n"
						));
						ImGui::Animations::ColorPicker(XOR("Tracers color"), &vars::visuals->world->tracer->beamTracer.color);
						ImGui::Animations::SliderFloat(XOR("Tracers life"), &vars::visuals->world->tracer->beamTracer.life, 0.0f, 10.0f);
						ImGui::Animations::SliderFloat(XOR("Tracers width"), &vars::visuals->world->tracer->beamTracer.width, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat(XOR("Tracers fadeLenght"), &vars::visuals->world->tracer->beamTracer.fadeLength, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat(XOR("Tracers amplitude"), &vars::visuals->world->tracer->beamTracer.amplitude, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat(XOR("Tracers speed"), &vars::visuals->world->tracer->beamTracer.speed, 0.0f, 200.0f);
						ImGui::Animations::SliderFloat(XOR("Tracers startFrame"), &vars::visuals->world->tracer->beamTracer.startFrame, 0.0f, 300.0f);
						ImGui::Animations::SliderFloat(XOR("Tracers frameRate"), &vars::visuals->world->tracer->beamTracer.frameRate, 0.0f, 200.0f);
						ImGui::SliderInt(XOR("Tracers segmnets"), &vars::visuals->world->tracer->beamTracer.segments, 0, 30);
					}
				);
				ImGui::Animations::Checkbox(XOR("Enable local bullets"), &vars::visuals->world->impacts->enabledLocal);
				ImGui::SameLine();			
				ImGui::Animations::Checkbox(XOR("Enable client bullets"), &vars::visuals->world->impacts->enabledClient);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##client bullets pop"), []()
					{
						ImGui::Animations::ColorPicker(XOR("Outline##Client bullets"), &vars::visuals->world->impacts->colorClient);
						ImGui::Animations::ColorPicker(XOR("Fill##Client bullets"), &vars::visuals->world->impacts->colorClientFill);
						ImGui::Animations::SliderFloat(XOR("Time##Client bullets"), &vars::visuals->world->impacts->timeClient, 0.0f, 5.0f);
					}
				);			

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel(XOR("Misc other"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Game logs"), &vars::misc->logs->enabled);
				ImGui::SameLine();
				ImGui::Animations::SliderFloat(XOR("Time##Gamelogs"), &vars::misc->logs->time, 1.0f, 10.0f);
				ImGui::Animations::Checkbox(XOR("Freelook"), &vars::misc->freeLook->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey(XOR("##fl"), &vars::keys->freeLook);
				ImGui::Animations::Checkbox(XOR("FreeCam"), &vars::misc->freeCam->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("##fc", &vars::keys->freeCam);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##freecam pop"), []()
					{
						ImGui::Animations::SliderFloat(XOR("Speed##fc"), &vars::misc->freeCam->speed, 1.0f, 20.0f);
					}
				);
				ImGui::Animations::Checkbox(XOR("MirrorCam"), &vars::misc->mirrorCam->enabled);
				ImGui::SameLine();
				ImGui::Animations::Checkbox(XOR("On key##mcam"), &vars::misc->mirrorCam->onKey);
				ImGui::SameLine();
				ImGui::Animations::Hotkey(XOR("##mcam"), &vars::keys->mirrorCam);
				ImGui::Animations::Checkbox(XOR("Flashlight"), &vars::misc->flashLight->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey(XOR("##flashl key"), &vars::keys->flashLight);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##flashlight pop"), []()
					{
						ImGui::Animations::Checkbox(XOR("Big mode"), &vars::misc->flashLight->bigMode);
						ImGui::Animations::SliderFloat(XOR("Flashlight FOV"), &vars::misc->flashLight->fov, 1.0f, 100.0f);
					}
				);
				ImGui::Animations::Checkbox(XOR("Fog enabled"), &vars::visuals->world->fog->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##fog pop"), []()
					{
						ImGui::Animations::SliderFloat(XOR("Fog distance"), &vars::visuals->world->fog->distance, 1.0f, 1000.0f);
						ImGui::Animations::ColorPicker(XOR("Fog color"), &vars::visuals->world->fog->color);
					}
				);
				ImGui::Animations::Combo<const char*>(XOR("Screen effect"), &vars::visuals->world->screenEffect->index, selections::screenEffects);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##screneffect pop"), []()
					{
						ImGui::Animations::SliderFloat(XOR("Param##Screen effect"), &vars::visuals->world->screenEffect->param, 0.0f, 1.0f);
						ImGui::Animations::ColorPicker(XOR("Color##Screen effect"), &vars::visuals->world->screenEffect->color);
					}
				);
				bool changedbut = false;
				changedbut |= ImGui::Animations::Checkbox(XOR("ControlTone enabled"), &vars::visuals->world->tone->enabled);
				g_ToneController->setStateButton(changedbut);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Tone control pop"), []()
					{
						bool changed = false;
						changed |= ImGui::Animations::SliderFloat(XOR("Tone min"), &vars::visuals->world->tone->min, 0.0f, 1.0f);
						changed |= ImGui::Animations::SliderFloat(XOR("Tone max"), &vars::visuals->world->tone->max, 0.0f, 1.0f);
						changed |= ImGui::Animations::SliderFloat(XOR("Tone bloom scale"), &vars::visuals->world->tone->bloom, 0.0f, 16.0f);
						g_ToneController->setStateSlider(changed);
					}
				);
				ImGui::Animations::Checkbox(XOR("Weather"), &vars::visuals->world->weather->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Weather control pop"), []()
					{
						g_WeatherController->implMenu();
					}
				);				
				ImGui::Animations::Checkbox(XOR("Motion blur"), &vars::misc->motionBlur->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton(XOR("##Motion blur pop"), []()
					{
						ImGui::Animations::Checkbox(XOR("Forward##Motion Blur"), &vars::misc->motionBlur->forward);
						ImGui::Animations::SliderFloat(XOR("Failling intensity##Motion Blur"), &vars::misc->motionBlur->fallingIntensity, 0.0f, 5.0f);
						ImGui::Animations::SliderFloat(XOR("Falling max##Motion Blur"), &vars::misc->motionBlur->fallingMax, 0.0f, 30.0f);
						ImGui::Animations::SliderFloat(XOR("Falling min##Motion Blur"), &vars::misc->motionBlur->fallingMin, 0.0f, 30.0f);
						ImGui::Animations::SliderFloat(XOR("Strength##Motion Blur"), &vars::misc->motionBlur->strength, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat(XOR("Roll intensity##Motion Blur"), &vars::misc->motionBlur->rollIntensity, 0.0f, 1.0f);
					}
				);
				bool changedbut2 = false;
				changedbut2 |= ImGui::Animations::Checkbox(XOR("Ambient"), &vars::visuals->world->ambient->enabled);
				g_AmbientLight->setButtonState(changedbut2);
				ImGui::SameLine();
				bool changed = false;
				changed |= ImGui::Animations::ColorPicker(XOR("Color##ambient col"), &vars::visuals->world->ambient->color);
				g_AmbientLight->setPickerState(changed);
				ImGui::Animations::Checkbox(XOR("Disable Interpolation"), &vars::misc->disableItems->interpolate);
				
				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}
	ImGui::Columns();
}

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
		if (ImGui::BeginChild(XOR("cfg"), {}, true))
		{
			const auto allcfg = config.getAllConfigFiles();

			ImGui::BeginGroupPanel(XOR("Config"), availRegion());
			{
				if (ImGui::InputText(XOR("Config name"), &text, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (utilities::toLowerCase(text) != config.getDefaultConfigName())
					{
						config.save(text);
						text.clear();
						config.reload();
					}
					else
						LOG_ERR(XOR("provided config name was same as default"));
				}
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("Press enter to create new config"));
				ImGui::Animations::ListBox(XOR("All configs"), &currentcfg, std::span(allcfg.data(), allcfg.size()));

				static bool dontAskMe = false;
				static bool delayedClose = false; // not instant close for modal 
				if (ImGui::Animations::Button(XOR("Delete")))
				{
					if (dontAskMe && delayedClose)
					{
						config.deleteCfg(allcfg.at(currentcfg));
						config.reload();
					}
					else
						ImGui::OpenPopup(XOR("Delete?"));
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::Animations::Button(XOR("Reset asking")))
					{
						dontAskMe = false;
						delayedClose = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				if (ImGui::BeginPopupModal(XOR("Delete?"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::TextUnformatted(FORMAT(XOR("Are you sure you want to delete {} file?"), allcfg.at(currentcfg)).c_str());
					ImGui::Separator();
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });
					ImGui::Animations::Checkbox(XOR("Don't ask again"), &dontAskMe);
					ImGui::PopStyleVar();

					if (ImGui::Animations::Button(XOR("OK"), { 120.0f, 0.0f }))
					{
						config.deleteCfg(allcfg.at(currentcfg));
						config.reload();
						ImGui::CloseCurrentPopup();

						if (dontAskMe)
							delayedClose = true;
						else
							delayedClose = false;
					}
					ImGui::SameLine();
					if (ImGui::Animations::Button(XOR("Cancel"), { 120.0f, 0.0f }))
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
				if (ImGui::Animations::Button(XOR("Save")))
				{
					config.save(allcfg.at(currentcfg));
				}
				ImGui::SameLine();
				if (ImGui::Animations::Button(XOR("Load")))
				{
					config.load(allcfg.at(currentcfg));
				}
				if (ImGui::Animations::Button(XOR("Load on start")))
				{
					// this is only saving the load name, nothing more
					config.startSave(allcfg.at(currentcfg));
					LOG_INFO(XOR("{} will be now loaded config on the start"), allcfg.at(currentcfg));
				}
				ImGui::SameLine();
				ImGui::HelpMarker(XOR("This config will load on the start"));
			}
			ImGui::EndGroupPanel();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	if (ImGui::BeginChild(XOR("cfgkeys"), {}, true))
	{
		ImGui::BeginGroupPanel(XOR("Config keys"), availRegion());
		{
			ImGui::Animations::Hotkey(XOR("Menu key"), &vars::keys->menu, false);
			ImGui::Animations::Hotkey(XOR("Console key"), &vars::keys->console, false);
			ImGui::Animations::Hotkey(XOR("Panic key"), &vars::keys->panic, false);
			ImGui::Animations::Checkbox(XOR("Enable x88 menu"), &vars::keys->enabledX88Menu);
		}
		ImGui::EndGroupPanel();
	}
	ImGui::EndChild();

	ImGui::Columns();
}

#include "background.hpp"

static bool editorOpened = false;

static void renderStyles()
{
	ImGui::Columns(1, nullptr, false);
	{
		if (ImGui::BeginChild(XOR("style"), {}, true))
		{
			ImGui::BeginGroupPanel(XOR("Style"), availRegion());
			{
				ImGui::Animations::Checkbox(XOR("Background"), &vars::styling->background);
				ImGui::Animations::SliderFloat(XOR("Background speed"), &vars::styling->speed, 1.0f, 8.0f);
				ImGui::Animations::SliderFloat(XOR("Background max dist"), &vars::styling->distance, 10.0f, 400.0f);
				ImGui::SliderInt(XOR("Background amount of records"), &vars::styling->size, 20, 400);
				ImGui::Text(XOR("They are temp limited with dynamic size + saving!"));
				ImGui::Animations::ColorPicker(XOR("Background color1"), &vars::styling->color1);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker(XOR("Background color2"), &vars::styling->color2);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker(XOR("Background color3"), &vars::styling->color3);
				if (ImGui::Animations::Button(XOR("Refresh background")))
					background.init();
				ImGui::Animations::Checkbox(XOR("Discord RPC"), &vars::misc->discord->enabled);
				ImGui::Animations::Checkbox(XOR("Show editor"), &editorOpened);
			}
			ImGui::EndGroupPanel();	
		}
		ImGui::EndChild();
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

void ImGuiMenu::renderAll()
{
	if(ImGui::Begin(XOR("csgo legit"), &m_active, ImGuiWindowFlags_NoCollapse))
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 backupPadding = style.FramePadding;
		float width = ImGui::GetContentRegionAvail().x;

		// remove tab underline
		ImGui::PushStyleColor(ImGuiCol_TabActive, Color::U32(Colors::Blank));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Color::U32(Colors::Blank));

		if (ImGui::BeginTabBar(XOR("tabbar"), ImGuiTabBarFlags_Reorderable))
		{
			style.FramePadding = { width / tabs.size(), backupPadding.y }; // still this is clamped by imgui in tabs
			ImGui::PopStyleColor(2);
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
	}
	ImGui::End();
}

bool ImGuiMenu::inTransmission()
{
	constexpr float ratio = 1.0f / 0.5f;
	float step = ratio * ImGui::GetIO().DeltaTime;
	const auto bgAlpha = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_WindowBg)).w;

	sharedAlpha = std::clamp(sharedAlpha + (m_active ? step : -step), 0.0f, bgAlpha);

	if (sharedAlpha == bgAlpha)
		return false;

	if (sharedAlpha == 0.0f)
		return false;

	return true;
}

void ImGuiMenu::draw()
{
	ImGui::Animations::setAnimationSpeeds();

	if (editorOpened)
	{
		if (ImGui::Begin(XOR("Style editor"), &editorOpened))
		{
			ImGui::ShowStyleEditorCfg(nullptr);

			ImGui::End();
		}
	}

	if (inTransmission())
	{
		ImGui::PushStyleColor(ImGuiStyleVar_Alpha, ImGui::GetColorU32(ImGuiStyleVar_Alpha, sharedAlpha));
		ImGui::SetNextWindowBgAlpha(sharedAlpha);
		renderAll();
		ImGui::PopStyleColor();

		return;
	}
	if (m_active)
		renderAll();
}