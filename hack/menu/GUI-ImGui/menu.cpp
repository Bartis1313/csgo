#include "menu.hpp"

#include "animations.hpp"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <implot.h>
#include <magic_enum.hpp>
#include <deps/magic_enum/prettyNames.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <render/render.hpp>
#include <utilities/console/console.hpp>
#include <cheats/features/visuals/chams/editor.hpp>
#include <config/vars.hpp>

#include <regex>
#include <fstream>

void ImGuiMenu::updateKeys()
{
	if (vars::keys->menu.isPressed())
		m_active = !m_active;
}

void ImGuiMenu::init()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	style = vars::styling->imStyle;

	m_iniFile = (config.getHackPath() / "window.ini").string();

	io.IniFilename = m_iniFile.c_str();
	io.LogFilename = nullptr;

	try
	{
		imRender.init(io);
	}
	catch (const std::runtime_error& err)
	{
		console::error(err.what());
	}
}

void ImGuiMenu::shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ImPlot::DestroyContext();
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
		if (ImGui::BeginChild("aim main", {}, true))
		{
			ImGui::BeginGroupPanel("Aimbot", availRegion());
			{
				ImGui::Animations::ListBox("Weapon selection", &index, magic_enum::enum_names_pretty<AimbotWeapon>());
				ImGui::Animations::Checkbox("Enabled##aim", &cfg.enabled);
				ImGui::SameLine();
				ImGui::Animations::Checkbox("Use key##aimbot", &vars::aim->useKey);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("Aimkey", &vars::keys->aimbot);
				ImGui::Animations::SliderFloat("Fov##aim", &cfg.fov, 0.0f, 50.0f);
				ImGui::Animations::Combo("Method##aim", &cfg.methodAim, magic_enum::enum_names_pretty<AimbotMethod>());
				ImGui::Animations::Combo("Hitboxes##aim", &cfg.aimSelection, magic_enum::enum_names_pretty<AimbotHitboxes>());
				ImGui::Animations::SliderFloat("Multiply##aim", &cfg.frametimeMulttiply, 0.0f, 30.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
				ImGui::SameLine();
				ImGui::HelpMarker("Scales with the frametime, more = faster");
				ImGui::Animations::Checkbox("Delay##aim", &cfg.aimDelay);
				ImGui::Animations::SliderFloat("Delay ms##aim", &cfg.aimDelayVal, 0.0f, 800.0f);
				ImGui::Animations::Checkbox("Aim at Backtrack", &cfg.aimBacktrack);
				ImGui::SameLine();
				ImGui::HelpMarker("Will aim at middle of records!");
				ImGui::Animations::Checkbox("Smoke check##Aimbot", &cfg.smokeCheck);
				ImGui::Animations::SliderFloat("Flash limit##Aimbot", &cfg.flashLimit, 0.0f, 255.0f);

				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}

	ImGui::NextColumn();

	if (ImGui::BeginChild("aim extra", {}, true, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::BeginGroupPanel("Backtracking & Latency", availRegion());
		{
			ImGui::Animations::Checkbox("Backtrack##enabled", &vars::backtrack->enabled);
			ImGui::Animations::SliderFloat("Backtrack ms", &vars::backtrack->time, 0.0f, 200.0f);
			ImGui::Animations::Checkbox("Smoke check##Backtrack", &vars::backtrack->smoke);
			ImGui::Animations::SliderFloat("Flash limit##Backtrack", &vars::backtrack->flashLimit, 0.0f, 255.0f);
			ImGui::Animations::Checkbox("Latency##enabled", &vars::misc->fakeLatency->enabled);
			ImGui::Animations::SliderFloat("Fake latency ms", &vars::misc->fakeLatency->amount, 0.0f, 200.0f);

			ImGui::EndGroupPanel();
		}

		ImGui::BeginGroupPanel("Triggerbot", availRegion());
		{
			ImGui::Animations::Checkbox("Triggerbot enabled", &cfg.triggerbot);
			ImGui::Animations::SliderFloat("Triggerbot ms", &cfg.triggerbotDelay, 0.0f, 200.0f);

			ImGui::EndGroupPanel();
		}

		ImGui::BeginGroupPanel("RCS", availRegion());
		{
			ImGui::Animations::Checkbox("Enabled##RCS", &cfg.rcs);
			ImGui::Animations::SliderFloat("Pitch##Rcsx", &cfg.rcsX, 0.0f, 1.0f);
			ImGui::Animations::SliderFloat("Yaw##Rcsy", &cfg.rcsY, 0.0f, 1.0f);

			ImGui::EndGroupPanel();
		}

		ImGui::BeginGroupPanel("Drawing", availRegion());
		{
			ImGui::Animations::Checkbox("Draw fov", &vars::aimPaint->enabledFov);
			ImGui::SameLine();
			ImGui::Animations::ColorPicker("Fov circle color", &vars::aimPaint->colorFov);
			ImGui::SameLine();
			ImGui::HelpMarker("Draws a circle representing your aimbot FOV");
			ImGui::Animations::Checkbox("Draw aimbot point", &vars::aimPaint->enabledPoint);
			ImGui::SameLine();
			ImGui::Animations::ColorPicker("Color##bestpoint", &vars::aimPaint->colorPoint);

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
		if (ImGui::BeginChild("visuals", {}, true))
		{
			ImGui::BeginGroupPanel("Players", availRegion());
			{
				ImGui::Animations::Checkbox("Enabled", &vars::visuals->esp->boxes->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Player boxes", []()
					{
						ImGui::Animations::Combo("Boxes mode", &vars::visuals->esp->boxes->mode, magic_enum::enum_names_pretty<BoxTypes>(), 40);
						ImGui::Animations::ColorPicker("color##box", &vars::visuals->esp->boxes->color);
						ImGui::Animations::ColorPicker("filled##box", &vars::visuals->esp->boxes->fill);
						ImGui::Animations::Checkbox("Outlined", &vars::visuals->esp->boxes->outline);
						if (auto type = vars::visuals->esp->boxes->mode; type == E2T(BoxTypes::FILLED2D) || type == E2T(BoxTypes::FILLED3D))
						{
							ImGui::Animations::Checkbox("Multi color rect fill", &vars::visuals->esp->boxes->multiColor);
							ImGui::Animations::SliderFloat("Rect fill multigradient speed", &vars::visuals->esp->boxes->multiColorSpeed, 0.0f, 15.0f);
						}
					}
				);

				ImGui::Animations::Checkbox("Health", &vars::visuals->esp->healthBar->enabled);
				ImGui::Animations::Checkbox("Armor", &vars::visuals->esp->armorBar->enabled);
				ImGui::Animations::Checkbox("Name", &vars::visuals->esp->nameBar->enabled);
				ImGui::Animations::Checkbox("Enemy aiming warn", &vars::misc->aimWarn->enabled);
				ImGui::Animations::Checkbox("Skeleton", &vars::visuals->esp->skeleton->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Skeleton ESP options", []()
					{
						ImGui::Animations::ColorPicker("Skeleton color", &vars::visuals->esp->skeleton->color);
						ImGui::Animations::Checkbox("Draw debug points##skelet", &vars::visuals->esp->skeleton->showDebug);
					}
				);

				ImGui::Animations::Checkbox("Sound ESP", &vars::visuals->sound->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Sound ESP options", []()
					{
						ImGui::Animations::ColorPicker("Circles color", &vars::visuals->sound->color);
						ImGui::Animations::SliderFloat("Step time", &vars::visuals->sound->time, 1.0f, 20.0f);
						ImGui::Animations::SliderFloat("Step max distance", &vars::visuals->sound->maxDist, 5.0f, 500.0f);
						ImGui::Animations::SliderFloat("Lines info distance", &vars::visuals->sound->maxDistLine, 2.0f, 200.0f);
						ImGui::HelpMarker("Max pixels to decide to draw info from centre of screen to the best point");
						ImGui::Animations::ColorPicker("Lines info color", &vars::visuals->sound->colorLine);
					}
				);
				ImGui::Animations::Checkbox("Dlight", &vars::visuals->esp->dlight->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Dlight options", []()
					{
						ImGui::Animations::ColorPicker("Dlight color", &vars::visuals->esp->dlight->color);
						ImGui::Animations::SliderFloat("Dlight radius", &vars::visuals->esp->dlight->radius, 0.0f, 300.0f);
						ImGui::Animations::SliderFloat("Dlight exponent", &vars::visuals->esp->dlight->exponent, 0.0f, 12.0f);
						ImGui::Animations::SliderFloat("Dlight decay", &vars::visuals->esp->dlight->decay, 0.0f, 100.0f);
					}
				);
				ImGui::Animations::MultiCombo("Esp flags", magic_enum::enum_names_pretty<EspFlags>(), &vars::visuals->esp->flags->flags);
				ImGui::Animations::Checkbox("Visible only##Visuals", &vars::visuals->esp->checks->visible);
				ImGui::Animations::Checkbox("Dead only ##Visuals", &vars::visuals->esp->checks->dead);
				ImGui::Animations::Checkbox("Smoke check##Visuals", &vars::visuals->esp->checks->smoke);
				ImGui::Animations::SliderFloat("Flash limit##Visuals", &vars::visuals->esp->checks->flashLimit, 0.0f, 255.0f);
				ImGui::Animations::SliderFloat("Dormacy time##Visuals", &vars::visuals->dormacy->time, 0.0f, 15.0f);
				ImGui::Animations::SliderFloat("Dormacy time box##Visuals", &vars::visuals->dormacy->limit, 0.0f, 15.0f);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Chams", availRegion());
			{
				const auto materials = g_Chams->getMaterials();
				std::vector<std::string> names;
				std::vector<std::string> matNames(materials.size());
				for (size_t i = 0; const auto& mat : materials)
					matNames[i++] = mat.data.name;

				ImGui::Animations::Checkbox("Chams enabled Players", &vars::visuals->chams->players);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Chams options players", [matNames]()
					{
						ImGui::Animations::Combo("Chams type Players", &vars::visuals->chams->indexPlayers, matNames);
						ImGui::Animations::ColorPicker("Chams color Players", &vars::visuals->chams->colorPlayers);
						ImGui::Animations::Checkbox("Chams XQZ", &vars::visuals->chams->enabledXQZPlayers);
						ImGui::Animations::ColorPicker("Chams XQZ color", &vars::visuals->chams->colorXQZPlayers);
					});
				ImGui::Animations::Checkbox("Chams enabled Weapons", &vars::visuals->chams->enabledWeapons);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Chams options weapons", [matNames]()
					{
						ImGui::Animations::Combo("Chams type Weapons", &vars::visuals->chams->indexWeapons, matNames);
						ImGui::Animations::ColorPicker("Chams color Weapons", &vars::visuals->chams->colorWeapons);
						ImGui::Animations::Checkbox("Disable weapon", &vars::visuals->chams->weaponHide);
					});
				ImGui::Animations::Checkbox("Chams enabled Arms", &vars::visuals->chams->enabledArms);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Chams options arms", [matNames]()
					{
						ImGui::Animations::Combo("Chams type Arms", &vars::visuals->chams->indexArms, matNames);
						ImGui::Animations::ColorPicker("Chams color Arms", &vars::visuals->chams->colorArms);
						ImGui::Animations::Checkbox("Disable arms", &vars::visuals->chams->armsHide);
					});
				ImGui::Animations::Checkbox("Chams enabled Backtrack", &vars::visuals->chams->enabledBacktrack);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Chams options backtrack", [matNames]()
					{
						ImGui::Animations::Combo("Chams type Backtrack", &vars::visuals->chams->modeBacktrack, matNames);
						ImGui::Animations::ColorPicker("Backtrack chams col", &vars::visuals->chams->colorBacktrack);
						ImGui::Animations::Combo("Chams style Backtrack", &vars::visuals->chams->indexBacktrack, magic_enum::enum_names_pretty<BTChamsType>());
					});
				ImGui::Animations::Checkbox("Enabled glow", &vars::visuals->glow->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Glow options", []()
					{
						const static auto names = magic_enum::enum_names_pretty<GlowRenderStyle>();
						ImGui::Animations::MultiCombo("Type", names, &vars::visuals->glow->usedMats);
						ImGui::Animations::MultiCombo("Ignorez", names, &vars::visuals->glow->ignorez);
						ImGui::Animations::Checkbox("Full bloom", &vars::visuals->glow->fullBloom);
						ImGui::SameLine();
						ImGui::Animations::ColorPicker("Color", &vars::visuals->glow->colorPlayer);
						ImGui::Animations::Checkbox("Occluded", &vars::visuals->glow->occluded);
						ImGui::SameLine();
						ImGui::Animations::Checkbox("UnOccluded", &vars::visuals->glow->unoccluded);
						ImGui::Animations::SliderFloat("Exponent", &vars::visuals->glow->exponent, 0.0f, 10.0f);
						ImGui::Animations::SliderFloat("Saturation", &vars::visuals->glow->saturation, 0.0f, 50.0f);
						ImGui::Animations::SliderFloat("$C0_X (halo add screen)", &vars::visuals->glow->C0_X, 0.0f, 1.0f);
						ImGui::Animations::SliderFloat("Thickness", &vars::visuals->glow->thickness, 0.0f, 100.0f);
						if (vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::RIMGLOW3D)))
						{
							ImGui::Animations::SliderFloat("Pulse rim3d",
								&vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::RIM)), 1.0f, 50.0f);
						}

						if (vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT_PULSE)))
						{
							ImGui::Animations::SliderFloat("Pulse edge",
								&vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::EDGE)), 1.0f, 50.0f);
						}
					});

				if (ImGui::Animations::Button("Open editor##mat"))
				{
					g_MaterialEditor->changeState();
				}

				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		if (ImGui::BeginChild("wpns", {}, true))
		{
			ImGui::BeginGroupPanel("Weapons", availRegion());
			{
				ImGui::Animations::Checkbox("Enabled##weapons", &vars::visuals->esp->weaponBar->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Weapons text and bar", []()
					{
						ImGui::Animations::Checkbox("Translate name", &vars::visuals->esp->weaponBar->translate);
						ImGui::Animations::ColorPicker("Color for text", &vars::visuals->esp->weaponBar->text);
						ImGui::Animations::ColorPicker("Reload bar color", &vars::visuals->esp->weaponBar->bar);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Bomb", availRegion());
			{
				ImGui::Animations::Checkbox("Bomb info", &vars::visuals->world->bomb->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Bomb info draw", []()
					{
						ImGui::Animations::ColorPicker("C4 info", &vars::visuals->world->bomb->background);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Nades", availRegion());
			{
				ImGui::Animations::Checkbox("Enabled##nades", &vars::visuals->world->projectiles->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Nades drawing", []()
					{
						ImGui::Animations::ColorPicker("Flashbang color", &vars::visuals->world->projectiles->flash);
						ImGui::Animations::ColorPicker("Granede color", &vars::visuals->world->projectiles->nade);
						ImGui::Animations::ColorPicker("Molotov color", &vars::visuals->world->projectiles->molotov);
						ImGui::Animations::ColorPicker("Smoke color", &vars::visuals->world->projectiles->smoke);
						ImGui::Animations::ColorPicker("Decoy color", &vars::visuals->world->projectiles->decoy);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Dropped", availRegion());
			{
				ImGui::Animations::Checkbox("Enabled##dropped", &vars::visuals->esp->dropped->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Dropped weapons draw", []()
					{
						ImGui::Animations::MultiCombo("Dropped flags", magic_enum::enum_names_pretty<DroppedFlags>(), &vars::visuals->esp->dropped->flags);
						ImGui::Animations::ColorPicker("Dropped color", &vars::visuals->esp->dropped->color);
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Particles edit", availRegion());
			{
				ImGui::Animations::Checkbox("##Edit molotov", &vars::visuals->world->particles->enabledMolotov);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker("Molotov##edited", &vars::visuals->world->particles->colorMolotov);
				ImGui::Animations::Checkbox("##Edit blood", &vars::visuals->world->particles->enabledBlood);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker("Blood##edited", &vars::visuals->world->particles->colorBlood);
				ImGui::Animations::Checkbox("##Edit smoke", &vars::visuals->world->particles->enabledSmoke);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker("Smoke##edited", &vars::visuals->world->particles->colorSmoke);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Molotov & Smoke range", availRegion());
			{
				ImGui::Animations::Checkbox("##molotov polygon", &vars::visuals->world->molotov->enabled);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker("Molotov##molrange", &vars::visuals->world->molotov->color);
				ImGui::SameLine();
				ImGui::Animations::Checkbox("Triangulation", &vars::visuals->world->molotov->triangulation);
				ImGui::Animations::Checkbox("##Enabled smoke circle", &vars::visuals->world->smoke->enabled);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker("Smoke##smokerange", &vars::visuals->world->smoke->color);

				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}
	ImGui::Columns();
}

#include <cheats/features/visuals/radar/radar.hpp>
#include <cheats/features/visuals/world/skybox.hpp>
#include <cheats/features/visuals/world/weather.hpp>
#include <cheats/features/visuals/world/ambient.hpp>
#include <cheats/features/visuals/world/tone.hpp>

static void renderMisc()
{
	ImGui::Columns(2, nullptr, false);
	{
		if (ImGui::BeginChild("Misc", {}, true))
		{
			ImGui::BeginGroupPanel("Skybox", availRegion());
			{
				const auto customsky = g_SkyboxEdit->getAllCustomSkyBoxes();
				bool state;
				state |= ImGui::Animations::Combo("Normal", &vars::visuals->world->sky->indexNormal, std::span(selections::skyboxes.data(), selections::skyboxes.size()));
				state |= ImGui::Animations::Combo("Custom##Skybox", &vars::visuals->world->sky->indexCustom, customsky);
				g_SkyboxEdit->handleButtonState(state);
				if (ImGui::Animations::Button("Reload Custom Skybox"))
				{
					g_SkyboxEdit->reloadCustomSkyboxes();
				}

				ImGui::EndGroupPanel();
			}
			ImGui::BeginGroupPanel("World Modulation", availRegion());
			{
				ImGui::Animations::Checkbox("Modulate world", &vars::visuals->world->modulate->enabled);
				ImGui::Animations::Checkbox("Remove sky", &vars::visuals->world->sky->removeSky);
				ImGui::Animations::ColorPicker("Modulate world texture", &vars::visuals->world->modulate->texture);
				ImGui::Animations::ColorPicker("Modulate world prop", &vars::visuals->world->modulate->prop);
				ImGui::Animations::ColorPicker("Modulate world sky", &vars::visuals->world->modulate->sky);
				ImGui::Animations::SliderFloat("Shader value", &vars::visuals->world->modulate->shader, 0.0f, 100.0f);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("View", availRegion());
			{
				ImGui::Animations::SliderFloat("FOV local", &vars::misc->fov->value, -50.0f, 50.0f);
				ImGui::Animations::Checkbox("Third Person", &vars::misc->thirdp->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("ThirdP", &vars::keys->thirdP);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Third person pop", []()
					{
						ImGui::Animations::SliderFloat("Distance##thirdp", &vars::misc->thirdp->distance, 1.0f, 500.0f);
						ImGui::Animations::SliderFloat("Extra X##thirdp", &vars::misc->thirdp->x, -180.0f, 180.0f);
						ImGui::Animations::SliderFloat("Extra Y##thirdp", &vars::misc->thirdp->y, -180.0f, 180.0f);
					}
				);
				ImGui::Animations::Combo("Crosshair type", &vars::misc->crosshair->index, magic_enum::enum_names_pretty<CrossHairTypes>());

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Radar", availRegion());
			{
				ImGui::Animations::Checkbox("2D Radar enabled", &vars::misc->radar->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##2D Radar pop", []()
					{
						ImGui::Animations::ColorPicker("Angle view line", &vars::misc->radar->colorLine);
						ImGui::Animations::ColorPicker("Players color", &vars::misc->radar->colorPlayer);
						ImGui::Animations::SliderFloat("Thickness", &vars::misc->radar->thickness, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat("Length", &vars::misc->radar->length, 0.0f, 40.0f);
						ImGui::Animations::SliderFloat("Scale", &vars::misc->radar->scale, 0.0f, 10.0f);
						ImGui::Animations::SliderFloat("Radar size", &vars::misc->radar->size, 10.0f, 500.0f);
						ImGui::Animations::Checkbox("Draw out of radar", &vars::misc->radar->ranges);
						ImGui::SameLine();
						ImGui::HelpMarker("If enemy is out of the radar\nThen icons will still appear but clamped");
					}
				);
				if (ImGui::Animations::Button("Refresh texture manually"))
				{
					g_Radar->manuallyInitTexture();
				}
				ImGui::SameLine();
				ImGui::HelpMarker("Will not work for workshop maps\nYou can try forcing the engine to re-render by pressing escape few times");
			}
			ImGui::EndGroupPanel();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		if (ImGui::BeginChild("Miscother", {}, true))
		{
			ImGui::BeginGroupPanel("Movement", availRegion());
			{
				ImGui::Animations::Checkbox("Bunnyhop", &vars::misc->bunnyHop->enabled);
				if(vars::misc->bunnyHop->enabled)
				{
					ImGui::SliderInt("Bunnyhop hitchance", &vars::misc->bunnyHop->chance, 0, 100, "%i%%");
				}
				ImGui::Animations::Combo("Autostrafe", &vars::misc->bunnyHop->indexStrafe, magic_enum::enum_names_pretty<MovementStraferMode>());

				ImGui::EndGroupPanel();
			}
			ImGui::BeginGroupPanel("Hitmarker", availRegion());
			{
				ImGui::Animations::Checkbox("Hitmarker", &vars::misc->hitmarker->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##hitmarker pop", []()
					{
						ImGui::Animations::Checkbox("3D##Hitm", &vars::misc->hitmarker->enabled3D);
						ImGui::SameLine();
						ImGui::Animations::Checkbox("Resize##Hitm", &vars::misc->hitmarker->enabledResize);
						ImGui::Animations::ColorPicker("Hitmarker normal", &vars::misc->hitmarker->colorNormal);
						ImGui::Animations::ColorPicker("Hitmarker hs", &vars::misc->hitmarker->colorHead);
						ImGui::SameLine();
						ImGui::Text("Hitmarker hs");
						ImGui::Animations::ColorPicker("Hitmarker dead", &vars::misc->hitmarker->colorDead);
						ImGui::Animations::SliderFloat("Hitmarker time", &vars::misc->hitmarker->time, 0.0f, 2.0f);
						ImGui::Animations::Checkbox("Play hitmarker", &vars::misc->hitmarker->play);
						ImGui::SameLine();
						ImGui::HelpMarker("Will play the sound after every kill");
					}
				);

				ImGui::EndGroupPanel();
			}
			ImGui::BeginGroupPanel("Misc", availRegion());
			{
				ImGui::Animations::Checkbox("No scope", &vars::misc->scope->enabled);
				
				ImGui::Animations::Checkbox("FPS Plot", &vars::misc->plots->enabledFps);				
				ImGui::Animations::Checkbox("Velocity Plot", &vars::misc->plots->enabledVelocity);
				ImGui::SameLine();
				ImGui::Animations::Checkbox("Run transparent", &vars::misc->plots->transparencyVelocity);
				ImGui::SameLine();
				ImGui::HelpMarker("Will add some flags!\nEg: no resize");

				ImGui::Animations::Checkbox("Draw misc info", &vars::misc->info->enabled);
				ImGui::Animations::Checkbox("Playerlist##checkbox", &vars::misc->playerList->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Playerlist pop", []()
					{
						ImGui::Animations::Checkbox("Health##playerlist", &vars::misc->playerList->health);
						ImGui::Animations::Checkbox("Money##playerlist", &vars::misc->playerList->money);
						ImGui::Animations::Checkbox("Team##playerlist", &vars::misc->playerList->teamID);
						ImGui::Animations::Checkbox("Place##playerlist", &vars::misc->playerList->lastPlace);
					}
				);
				ImGui::Animations::Checkbox("Spectactors##specson", &vars::misc->spectactorList->enabled);
				ImGui::Animations::Checkbox("Hat on local", &vars::misc->hat->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Hat pop", []()
					{
						ImGui::Animations::Checkbox("Rainbow hat", &vars::misc->hat->rainbow);
						if (!vars::misc->hat->rainbow)
						{
							ImGui::Animations::ColorPicker("Hat triangle color", &vars::misc->hat->colorTriangle);
							ImGui::Animations::ColorPicker("Hat lines color", &vars::misc->hat->colorLine);
						}
						else
						{
							ImGui::Animations::SliderFloat("Hat rainbow speed", &vars::misc->hat->rainbowSpeed, 0.2f, 10.0f);
						}
						ImGui::Animations::SliderFloat("Hat size", &vars::misc->hat->size, -100.0f, 100.0f);
						ImGui::Animations::SliderFloat("Hat radius", &vars::misc->hat->radius, 1.0f, 100.0f);
					}
				);

				ImGui::EndGroupPanel();
			}
			ImGui::BeginGroupPanel("Nade prediction", availRegion());
			{
				ImGui::Animations::Checkbox("Nade pred", &vars::misc->nade->enabledPred);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##nade pred pop", []()
					{
						ImGui::Animations::Checkbox("Show always", &vars::misc->nade->predAlways);
						ImGui::Animations::ColorPicker("Nade pred color", &vars::misc->nade->colorPredLine);
						ImGui::Animations::ColorPicker("Nade pred color box fill", &vars::misc->nade->colorPredBoxFill);
						ImGui::Animations::ColorPicker("Nade pred color box outline", &vars::misc->nade->colorPredBox);
					}
				);
				ImGui::Animations::Checkbox("Nade trails", &vars::misc->nade->enabledTracer);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##nade trails pop", []()
					{
						ImGui::Animations::ColorPicker("Nade trail color", &vars::misc->nade->colorTracer);
						ImGui::Animations::SliderFloat("Nade trail max dist", &vars::misc->nade->tracerDist, 1.0f, 100.0f);
						ImGui::Animations::Checkbox("Nade trail warn", &vars::misc->nade->tracerWarn);
						ImGui::SameLine();
						ImGui::HelpMarker("Draws scaled circles behind you where nade is");
					}
				);

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Bullets and trails", availRegion());
			{
				ImGui::Animations::Checkbox("Movement trail", &vars::misc->trail->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Movement trails pop", []()
					{
						ImGui::Animations::Combo("Trail type", &vars::misc->trail->mode, magic_enum::enum_names_pretty<MovementTrail>());
						ImGui::Animations::ColorPicker("Movement trail color", &vars::misc->trail->color);
						ImGui::Animations::SliderFloat("Trail speed", &vars::misc->trail->beamSpeed, 1.0f, 10.0f);
						ImGui::Animations::SliderFloat("Trail life", &vars::misc->trail->time, 1.0f, 10.0f);
					}
				);

				ImGui::Animations::Checkbox("Enable tracers", &vars::visuals->world->tracer->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Bullet tracers", []()
					{
						ImGui::Animations::Combo("Beam sprite", &vars::visuals->world->tracer->beamTracer.index, selections::beamNames);
						ImGui::InputTextWithHint("Beam flags", "eg: 4|8, spaces allowed", &vars::visuals->world->tracer->beamTracer.flags);
						ImGui::SameLine();
						ImGui::HelpMarker("Flags list\n"
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
						);
						ImGui::Animations::ColorPicker("Tracers color", &vars::visuals->world->tracer->beamTracer.color);
						ImGui::Animations::SliderFloat("Tracers life", &vars::visuals->world->tracer->beamTracer.life, 0.0f, 10.0f);
						ImGui::Animations::SliderFloat("Tracers width", &vars::visuals->world->tracer->beamTracer.width, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat("Tracers fadeLenght", &vars::visuals->world->tracer->beamTracer.fadeLength, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat("Tracers amplitude", &vars::visuals->world->tracer->beamTracer.amplitude, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat("Tracers speed", &vars::visuals->world->tracer->beamTracer.speed, 0.0f, 200.0f);
						ImGui::Animations::SliderFloat("Tracers startFrame", &vars::visuals->world->tracer->beamTracer.startFrame, 0.0f, 300.0f);
						ImGui::Animations::SliderFloat("Tracers frameRate", &vars::visuals->world->tracer->beamTracer.frameRate, 0.0f, 200.0f);
						ImGui::SliderInt("Tracers segmnets", &vars::visuals->world->tracer->beamTracer.segments, 0, 30);
					}
				);	
				ImGui::Animations::Checkbox("Enable client bullets", &vars::visuals->world->impacts->enabledClient);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##client bullets pop", []()
					{
						ImGui::Animations::ColorPicker("Outline##Client bullets", &vars::visuals->world->impacts->colorClient);
						ImGui::Animations::ColorPicker("Fill##Client bullets", &vars::visuals->world->impacts->colorClientFill);
						ImGui::Animations::SliderFloat("Time##Client bullets", &vars::visuals->world->impacts->timeClient, 0.0f, 5.0f);
					}
				);			

				ImGui::EndGroupPanel();
			}

			ImGui::BeginGroupPanel("Misc other", availRegion());
			{
				ImGui::Animations::Checkbox("Game logs", &vars::misc->logs->enabled);
				ImGui::SameLine();
				ImGui::Animations::SliderFloat("Time##Gamelogs", &vars::misc->logs->time, 1.0f, 10.0f);
				ImGui::Animations::Checkbox("Freelook", &vars::misc->freeLook->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("##fl", &vars::keys->freeLook);
				ImGui::Animations::Checkbox("FreeCam", &vars::misc->freeCam->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("##fc", &vars::keys->freeCam);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##freecam pop", []()
					{
						ImGui::Animations::SliderFloat("Speed##fc", &vars::misc->freeCam->speed, 1.0f, 20.0f);
					}
				);
				ImGui::Animations::Checkbox("MirrorCam", &vars::misc->mirrorCam->enabled);
				ImGui::SameLine();
				ImGui::Animations::Checkbox("On key##mcam", &vars::misc->mirrorCam->onKey);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("##mcam", &vars::keys->mirrorCam);
				ImGui::Animations::Checkbox("Flashlight", &vars::misc->flashLight->enabled);
				ImGui::SameLine();
				ImGui::Animations::Hotkey("##flashl key", &vars::keys->flashLight);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##flashlight pop", []()
					{
						ImGui::Animations::Checkbox("Big mode", &vars::misc->flashLight->bigMode);
						ImGui::Animations::SliderFloat("Flashlight FOV", &vars::misc->flashLight->fov, 1.0f, 100.0f);
					}
				);
				ImGui::Animations::Checkbox("Fog enabled", &vars::visuals->world->fog->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##fog pop", []()
					{
						ImGui::Animations::SliderFloat("Fog distance", &vars::visuals->world->fog->distance, 1.0f, 1000.0f);
						ImGui::Animations::ColorPicker("Fog color", &vars::visuals->world->fog->color);
					}
				);
				ImGui::Animations::Combo("Screen effect", &vars::visuals->world->screenEffect->index, selections::screenEffects);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##screneffect pop", []()
					{
						ImGui::Animations::SliderFloat("Param##Screen effect", &vars::visuals->world->screenEffect->param, 0.0f, 1.0f);
						ImGui::Animations::ColorPicker("Color##Screen effect", &vars::visuals->world->screenEffect->color);
					}
				);
				bool changedbut = false;
				changedbut |= ImGui::Animations::Checkbox("ControlTone enabled", &vars::visuals->world->tone->enabled);
				g_ToneController->setStateButton(changedbut);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Tone control pop", []()
					{
						bool changed = false;
						changed |= ImGui::Animations::SliderFloat("Tone min", &vars::visuals->world->tone->min, 0.0f, 1.0f);
						changed |= ImGui::Animations::SliderFloat("Tone max", &vars::visuals->world->tone->max, 0.0f, 1.0f);
						changed |= ImGui::Animations::SliderFloat("Tone bloom scale", &vars::visuals->world->tone->bloom, 0.0f, 16.0f);
						g_ToneController->setStateSlider(changed);
					}
				);
				ImGui::Animations::Checkbox("Weather", &vars::visuals->world->weather->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Weather control pop", []()
					{
						g_WeatherController->implMenu();
					}
				);				
				ImGui::Animations::Checkbox("Motion blur", &vars::misc->motionBlur->enabled);
				ImGui::SameLine();
				ImGui::Animations::PopupButton("##Motion blur pop", []()
					{
						ImGui::Animations::Checkbox("Forward##Motion Blur", &vars::misc->motionBlur->forward);
						ImGui::Animations::SliderFloat("Failling intensity##Motion Blur", &vars::misc->motionBlur->fallingIntensity, 0.0f, 5.0f);
						ImGui::Animations::SliderFloat("Falling max##Motion Blur", &vars::misc->motionBlur->fallingMax, 0.0f, 30.0f);
						ImGui::Animations::SliderFloat("Falling min##Motion Blur", &vars::misc->motionBlur->fallingMin, 0.0f, 30.0f);
						ImGui::Animations::SliderFloat("Strength##Motion Blur", &vars::misc->motionBlur->strength, 0.0f, 20.0f);
						ImGui::Animations::SliderFloat("Roll intensity##Motion Blur", &vars::misc->motionBlur->rollIntensity, 0.0f, 1.0f);
					}
				);
				bool changedbut2 = false;
				changedbut2 |= ImGui::Animations::Checkbox("Ambient", &vars::visuals->world->ambient->enabled);
				g_AmbientLight->setButtonState(changedbut2);
				ImGui::SameLine();
				bool changed = false;
				changed |= ImGui::Animations::ColorPicker("Color##ambient col", &vars::visuals->world->ambient->color);
				g_AmbientLight->setPickerState(changed);
				ImGui::Animations::Checkbox("Disable Interpolation", &vars::misc->disableItems->interpolate);
				ImGui::Animations::SliderFloat("Flashbang %", &vars::misc->disableItems->flashPercentage, 0.0f, 1.0f);
				
				ImGui::EndGroupPanel();
			}
		}
		ImGui::EndChild();
	}
	ImGui::Columns();
}

static void renderConfig()
{
	static std::string text = "Your new config name";
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
		if (ImGui::BeginChild("cfg", {}, true))
		{
			const auto allcfg = config.getAllConfigFiles();

			ImGui::BeginGroupPanel("Config", availRegion());
			{
				if (ImGui::InputText("Config name", &text, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (utilities::toLowerCase(text) != config.getDefaultConfigName())
					{
						config.save(text);
						text.clear();
						config.reload();
					}
					else
						console::error("provided config name was same as default");
				}
				ImGui::SameLine();
				ImGui::HelpMarker("Press enter to create new config");
				ImGui::Animations::ListBox("All configs", &currentcfg, std::span(allcfg.data(), allcfg.size()));

				static bool dontAskMe = false;
				static bool delayedClose = false; // not instant close for modal 
				if (ImGui::Animations::Button("Delete"))
				{
					if (dontAskMe && delayedClose)
					{
						config.deleteCfg(allcfg.at(currentcfg));
						config.reload();
					}
					else
						ImGui::OpenPopup("Delete?");
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::Animations::Button("Reset asking"))
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
					ImGui::Animations::Checkbox("Don't ask again", &dontAskMe);
					ImGui::PopStyleVar();

					if (ImGui::Animations::Button("OK", { 120.0f, 0.0f }))
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
					if (ImGui::Animations::Button("Cancel", { 120.0f, 0.0f }))
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
				if (ImGui::Animations::Button("Save"))
				{
					config.save(allcfg.at(currentcfg));
				}
				ImGui::SameLine();
				if (ImGui::Animations::Button("Load"))
				{
					config.load(allcfg.at(currentcfg));
				}
				if (ImGui::Animations::Button("Load on start"))
				{
					// this is only saving the load name, nothing more
					config.startSave(allcfg.at(currentcfg));
					console::info("{} will be now loaded config on the start", allcfg.at(currentcfg));
				}
				ImGui::SameLine();
				ImGui::HelpMarker("This config will load on the start");
			}
			ImGui::EndGroupPanel();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	if (ImGui::BeginChild("cfgkeys", {}, true))
	{
		ImGui::BeginGroupPanel("Config keys", availRegion());
		{
			ImGui::Animations::Hotkey("Menu key", &vars::keys->menu, false);
			ImGui::Animations::Hotkey("Console key", &vars::keys->console, false);
			ImGui::Animations::Hotkey("Panic key", &vars::keys->panic, false);
			ImGui::Animations::Checkbox("Enable x88 menu", &vars::keys->enabledX88Menu);
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
		if (ImGui::BeginChild("style", {}, true))
		{
			ImGui::BeginGroupPanel("Style", availRegion());
			{
				ImGui::Animations::Checkbox("Background", &vars::styling->background);
				ImGui::Animations::SliderFloat("Background speed", &vars::styling->speed, 1.0f, 8.0f);
				ImGui::Animations::SliderFloat("Background max dist", &vars::styling->distance, 10.0f, 400.0f);
				ImGui::SliderInt("Background amount of records", &vars::styling->size, 20, 400);
				ImGui::Text("They are temp limited with dynamic size + saving!");
				ImGui::Animations::ColorPicker("Background color1", &vars::styling->color1);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker("Background color2", &vars::styling->color2);
				ImGui::SameLine();
				ImGui::Animations::ColorPicker("Background color3", &vars::styling->color3);
				if (ImGui::Animations::Button("Refresh background"))
					g_Background->init();
				ImGui::Animations::Checkbox("Discord RPC", &vars::misc->discord->enabled);
				ImGui::Animations::Checkbox("Show editor", &editorOpened);
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


// custom imgui helpers

bool isResized()
{
	if (!ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		return false;

	static ImVec2 lastSize;
	const ImVec2& currentSize = ImGui::GetCurrentWindow()->Size;
	if (lastSize.x != currentSize.x || lastSize.y != currentSize.y)
	{
		lastSize = currentSize;
		return true;
	}

	return false;
}

bool isMovedPos()
{
	if (GImGui->MovingWindow == nullptr)
		return false;

	return true;
}

void ImGuiMenu::renderAll()
{
	if (ImGui::Begin(m_menuTitle, &m_active, ImGuiWindowFlags_NoCollapse))
	{
		m_windowSize = ImLerp(m_windowSize, m_active ? m_targetSize : ImVec2{}, ImGui::GetIO().DeltaTime * 8.0f);
		m_centrePos = m_windowPos - (m_windowSize / 2);

		const bool sized = isResized();
		const bool posed = isMovedPos();

		if (sized || posed)
		{
			m_targetSize = ImGui::GetWindowSize();
			m_windowPos = ImGui::GetWindowPos() + m_targetSize / 2;
		}

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 backupPadding = style.FramePadding;
		float width = ImGui::GetContentRegionAvail().x;

		// remove tab underline
		ImGui::PushStyleColor(ImGuiCol_TabActive, Color::U32(Colors::Blank));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Color::U32(Colors::Blank));

		if (ImGui::BeginTabBar("tabbar", ImGuiTabBarFlags_Reorderable))
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


void ImGuiMenu::draw()
{
	//static std::once_flag once;
	//std::call_once(once, []() {ImGui::LoadCustomSettings(); });

	//ImGui::Animations::setAnimationSpeeds();

	//if (editorOpened)
	//{
	//	if (ImGui::Begin("Style editor", &editorOpened))
	//	{
	//		ImGui::ShowStyleEditorCfg(nullptr);

	//		ImGui::End();
	//	}
	//}

	//ImGui::Animations::Window(m_menuTitle, &m_active,
	//	ImGui::Animations::WindowConfig
	//	{
	//		.flags = ImGuiWindowFlags_NoCollapse,
	//		.defaultPos = {400, 300},
	//		.defaultSize = {250, 500}
	//	}, [this]()
	//	{
	//		ImGuiStyle& style = ImGui::GetStyle();
	//ImVec2 backupPadding = style.FramePadding;
	//float width = ImGui::GetContentRegionAvail().x;

	//// remove tab underline
	//ImGui::PushStyleColor(ImGuiCol_TabActive, Color::U32(Colors::Blank));
	//ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Color::U32(Colors::Blank));

	//if (ImGui::BeginTabBar("tabbar", ImGuiTabBarFlags_Reorderable))
	//{
	//	style.FramePadding = { width / tabs.size(), backupPadding.y }; // still this is clamped by imgui in tabs
	//	ImGui::PopStyleColor(2);
	//	for (const auto& el : tabs)
	//	{
	//		if (ImGui::BeginTabItem(el.m_name))
	//		{
	//			style.FramePadding = backupPadding;
	//			if (el.funcExist())
	//				el.m_func();

	//			ImGui::EndTabItem();
	//		}
	//	}
	//	ImGui::EndTabBar();
	//}
	//	});

	ImGui::Animations::setAnimationSpeeds();

	if (editorOpened)
	{
		if (ImGui::Begin("Style editor", &editorOpened))
		{
			ImGui::ShowStyleEditorCfg(nullptr);

			ImGui::End();
		}
	}

	if (m_active)
		renderAll();
}