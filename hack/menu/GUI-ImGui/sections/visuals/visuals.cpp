#include "visuals.hpp"

#include "../common.hpp"
#include "../../selections.hpp"

#include <cheats/features/visuals/chams/chams.hpp>
#include <cheats/features/visuals/world/skybox.hpp>
#include <cheats/features/visuals/world/weather.hpp>
#include <cheats/features/visuals/world/ambient.hpp>
#include <cheats/features/visuals/radar/radar.hpp>

#include <imgui_stdlib.h>

void tabs::visuals::draw()
{
	ImGui::Columns(2, nullptr, false);
	ImGui::BeginChild("##visualsleft");
	{
		ImGui::BeginGroupPanel("Player", ImGui::GetContentRegionAvail());
		{
			ImGui::Combo("Boxes mode", &vars::visuals->esp->boxes->mode, magic_enum::enum_names_pretty<BoxTypes>());
			if (auto type = vars::visuals->esp->boxes->mode; type == E2T(BoxTypes::FILLED2D) || type == E2T(BoxTypes::FILLED3D))
			{
				ImGui::Checkbox("Multicolor##visuals", &vars::visuals->esp->boxes->multiColor);
				ImGui::SliderFloat("Speed##multicolor", &vars::visuals->esp->boxes->multiColorSpeed, 0.0f, 15.0f);
			}
			ImGui::ColorPicker("color##box", &vars::visuals->esp->boxes->color);
			ImGui::SameLine();
			ImGui::ColorPicker("filled##box", &vars::visuals->esp->boxes->fill);
			ImGui::SameLine();
			ImGui::Checkbox("Outlined", &vars::visuals->esp->boxes->outline);

			ImGui::Checkbox("Health", &vars::visuals->esp->healthBar->enabled);
			ImGui::SameLine();
			ImGui::Checkbox("Armor", &vars::visuals->esp->armorBar->enabled);
			ImGui::SameLine();
			ImGui::Checkbox("Name", &vars::visuals->esp->nameBar->enabled);
			ImGui::Checkbox("Enemy aiming warn", &vars::misc->aimWarn->enabled);
			ImGui::Checkbox("##Skeleton", &vars::visuals->esp->skeleton->enabled);
			ImGui::SameLine();
			ImGui::ColorPicker("Skeleton##color", &vars::visuals->esp->skeleton->color);
#ifdef _DEBUG
			ImGui::SameLine();
			ImGui::Checkbox("Debug##skelet", &vars::visuals->esp->skeleton->showDebug);
#endif
			ImGui::MultiCombo("Esp flags", magic_enum::enum_names_pretty<EspFlags>(), &vars::visuals->esp->flags->flags);
			ImGui::Checkbox("Weapon bar", &vars::visuals->esp->weaponBar->enabled);
			if (vars::visuals->esp->weaponBar->enabled)
			{
				ImGui::SameLine();
				ImGui::Checkbox("Translate", &vars::visuals->esp->weaponBar->translate);
				ImGui::ColorPicker("Text", &vars::visuals->esp->weaponBar->text);
				ImGui::SameLine();
				ImGui::ColorPicker("Bar", &vars::visuals->esp->weaponBar->bar);
			}
			ImGui::Checkbox("Sound ESP", &vars::visuals->sound->enabled);
			if (vars::visuals->sound->enabled)
			{
				ImGui::SameLine();
				ImGui::ColorPicker("Color##sound", &vars::visuals->sound->color);
				ImGui::SameLine();
				ImGui::ColorPicker("Line##sound", &vars::visuals->sound->colorLine);
				ImGui::SliderFloat("Step time", &vars::visuals->sound->time, 1.0f, 20.0f);
				ImGui::SliderFloat("Step max distance", &vars::visuals->sound->maxDist, 5.0f, 500.0f);
				ImGui::SliderFloat("Lines info distance", &vars::visuals->sound->maxDistLine, 2.0f, 200.0f);
				ImGui::SameLine();
				ImGui::HelpMarker("Max pixels to decide to draw info from centre of screen to the best point");
			}
			ImGui::Checkbox("Dlight", &vars::visuals->esp->dlight->enabled);
			if (vars::visuals->esp->dlight->enabled)
			{
				ImGui::SameLine();
				ImGui::ColorPicker("Color##dlig", &vars::visuals->esp->dlight->color);
				ImGui::SliderFloat("Radius##dlig", &vars::visuals->esp->dlight->radius, 0.0f, 300.0f);
				ImGui::SliderFloat("Exponent##dlig", &vars::visuals->esp->dlight->exponent, 0.0f, 12.0f);
				ImGui::SliderFloat("Decay##dlig", &vars::visuals->esp->dlight->decay, 0.0f, 100.0f);
			}
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Chams & Glow", ImGui::GetContentRegionAvail());
		{
			const auto materials = chams::materials;
			std::vector<std::string> names;
			std::vector<std::string> matNames(materials.size());
			for (size_t i = 0; const auto & mat : materials)
				matNames[i++] = mat.name;

			ImGui::Checkbox("Players##chams", &vars::visuals->chams->players);
			if (vars::visuals->chams->players)
			{
				ImGui::SameLine();
				ImGui::ColorPicker("Color##playercha", &vars::visuals->chams->colorPlayers);
				ImGui::SameLine();
				ImGui::Checkbox("XQZ##playercha", &vars::visuals->chams->enabledXQZPlayers);
				if (vars::visuals->chams->enabledXQZPlayers)
				{
					ImGui::SameLine();
					ImGui::ColorPicker("Color##playerchaxqz", &vars::visuals->chams->colorXQZPlayers);
				}
				ImGui::Combo("Type##playercha", &vars::visuals->chams->indexPlayers, matNames);
			}

			ImGui::Checkbox("Weapons##chams", &vars::visuals->chams->enabledWeapons);
			if (vars::visuals->chams->enabledWeapons)
			{
				ImGui::SameLine();
				ImGui::ColorPicker("Color##weapons", &vars::visuals->chams->colorWeapons);
				ImGui::Combo("Type##weapons", &vars::visuals->chams->indexWeapons, matNames);
				ImGui::Checkbox("Disable##weapon", &vars::visuals->chams->weaponHide);
			}
			ImGui::Checkbox("Arms##chams", &vars::visuals->chams->enabledArms);
			if (vars::visuals->chams->enabledArms)
			{
				ImGui::SameLine();
				ImGui::ColorPicker("Color##arms", &vars::visuals->chams->colorArms);
				ImGui::Combo("Type##arms", &vars::visuals->chams->indexArms, matNames);
				ImGui::Checkbox("Disable##arms", &vars::visuals->chams->armsHide);
			}
			ImGui::Checkbox("Backtrack##chams", &vars::visuals->chams->enabledBacktrack);
			if (vars::visuals->chams->enabledBacktrack)
			{
				ImGui::SameLine();
				ImGui::ColorPicker("Color##backtrack", &vars::visuals->chams->colorBacktrack);
				ImGui::Combo("Type##backtrack", &vars::visuals->chams->modeBacktrack, matNames);
				ImGui::Combo("Style##backtrack", &vars::visuals->chams->indexBacktrack, magic_enum::enum_names_pretty<BTChamsType>());
			}
			ImGui::Checkbox("Glow##enabled", &vars::visuals->glow->enabled);
			if (vars::visuals->glow->enabled)
			{
				const static auto names = magic_enum::enum_names_pretty<GlowRenderStyle>();
				ImGui::MultiCombo("Type", names, &vars::visuals->glow->usedMats);
				ImGui::MultiCombo("Ignorez", names, &vars::visuals->glow->ignorez);
				ImGui::Checkbox("Full bloom", &vars::visuals->glow->fullBloom);
				ImGui::SameLine();
				ImGui::ColorPicker("Color", &vars::visuals->glow->colorPlayer);
				ImGui::Checkbox("Occluded", &vars::visuals->glow->occluded);
				ImGui::SameLine();
				ImGui::Checkbox("UnOccluded", &vars::visuals->glow->unoccluded);
				ImGui::SliderFloat("Exponent", &vars::visuals->glow->exponent, 0.0f, 10.0f);
				ImGui::SliderFloat("Saturation", &vars::visuals->glow->saturation, 0.0f, 50.0f);
				ImGui::SliderFloat("$C0_X (halo add screen)", &vars::visuals->glow->C0_X, 0.0f, 1.0f);
				ImGui::SliderFloat("Thickness", &vars::visuals->glow->thickness, 0.0f, 100.0f);
				if (vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::RIMGLOW3D)))
				{
					ImGui::SliderFloat("Pulse rim3d",
						&vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::RIM)), 1.0f, 50.0f);
				}

				if (vars::visuals->glow->usedMats.at(E2T(GlowRenderStyle::EDGE_HIGHLIGHT_PULSE)))
				{
					ImGui::SliderFloat("Pulse edge",
						&vars::visuals->glow->pulseSpeeeds.at(E2T(GlowRenderPulse::EDGE)), 1.0f, 50.0f);
				}
			}

			ImGui::EndGroupPanel();
		}

		ImGui::BeginGroupPanel("Radar", ImGui::GetContentRegionAvail());
		{
			ImGui::Checkbox("Radar 2D", &vars::misc->radar->enabled);
			ImGui::SameLine();
			ImGui::Checkbox("Draw out of radar", &vars::misc->radar->ranges);
			ImGui::SameLine();
			ImGui::HelpMarker("If enemy is out of the radar\nThen icons will still appear but clamped");
			ImGui::ColorPicker("Lines##radar", &vars::misc->radar->colorLine);
			ImGui::SameLine();
			ImGui::ColorPicker("Players##radar", &vars::misc->radar->colorPlayer);
			ImGui::SliderFloat("Thickness##radar", &vars::misc->radar->thickness, 0.0f, 20.0f);
			if (vars::misc->radar->mode == E2T(RadarMode::CIRCLE))
			{
				ImGui::SliderFloat("Length##radar", &vars::misc->radar->length, 0.0f, 40.0f);
			}
			ImGui::SliderFloat("Scale##radar", &vars::misc->radar->scale, 0.0f, 10.0f);
			ImGui::Combo("Mode", &vars::misc->radar->mode, magic_enum::enum_names_pretty<RadarMode>());
			if (ImGui::Button("Refresh texture manually"))
			{
			//
			}
			ImGui::SameLine();
			ImGui::HelpMarker("Will not work for workshop maps\nYou can try forcing the engine to re-render by pressing escape few times");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Checks", ImGui::GetContentRegionAvail());
		{
			ImGui::Checkbox("Visible only##Visuals", &vars::visuals->esp->checks->visible);
			ImGui::Checkbox("Dead only ##Visuals", &vars::visuals->esp->checks->dead);
			ImGui::Checkbox("Smoke check##Visuals", &vars::visuals->esp->checks->smoke);
			ImGui::SliderFloat("Flash limit##Visuals", &vars::visuals->esp->checks->flashLimit, 0.0f, 255.0f);
			ImGui::SliderFloat("Dormacy time##Visuals", &vars::visuals->dormacy->time, 0.0f, 15.0f);
			ImGui::SliderFloat("Dormacy limit##Visuals", &vars::visuals->dormacy->limit, 0.0f, 15.0f);
		}
		ImGui::EndGroupPanel();
	}
	ImGui::EndChild();

	ImGui::NextColumn();

	ImGui::BeginChild("##rightvisuals");
	{

		ImGui::BeginGroupPanel("World", ImGui::GetContentRegionAvail());
		{
			ImGui::Checkbox("##Bombenabled", &vars::visuals->world->bomb->enabled);
			ImGui::SameLine();
			ImGui::ColorPicker("Bomb info", &vars::visuals->world->bomb->background);
			ImGui::Checkbox("Projectiles##enbaled", &vars::visuals->world->projectiles->enabled);
			if (vars::visuals->world->projectiles->enabled)
			{
				ImGui::ColorPicker("Flashbang##proj", &vars::visuals->world->projectiles->flash);
				ImGui::SameLine();
				ImGui::ColorPicker("Granede##proj", &vars::visuals->world->projectiles->nade);
				ImGui::SameLine();
				ImGui::ColorPicker("Molotov##proj", &vars::visuals->world->projectiles->molotov);
				ImGui::SameLine();
				ImGui::ColorPicker("Smoke##proj", &vars::visuals->world->projectiles->smoke);
				ImGui::SameLine();
				ImGui::ColorPicker("Decoy##proj", &vars::visuals->world->projectiles->decoy);
			}
			ImGui::Checkbox("Dropped##enabled", &vars::visuals->esp->dropped->enabled);
			if (vars::visuals->esp->dropped->enabled)
			{
				ImGui::MultiCombo("##Dropped flags", magic_enum::enum_names_pretty<DroppedFlags>(), &vars::visuals->esp->dropped->flags);
				ImGui::SameLine();
				ImGui::ColorPicker("Color##dropped", &vars::visuals->esp->dropped->color);
			}
			ImGui::Checkbox("##molotov polygon", &vars::visuals->world->molotov->enabled);
			ImGui::SameLine();
			ImGui::ColorPicker("Molotov##molrange", &vars::visuals->world->molotov->color);
			ImGui::SameLine();
			ImGui::Checkbox("Triangulation", &vars::visuals->world->molotov->triangulation);
			ImGui::Checkbox("##Enabled smoke circle", &vars::visuals->world->smoke->enabled);
			ImGui::SameLine();
			ImGui::ColorPicker("Smoke##smokerange", &vars::visuals->world->smoke->color);

			ImGui::Checkbox("Nade pred", &vars::misc->nade->enabledPred);
			ImGui::SameLine();
			ImGui::Checkbox("Show always", &vars::misc->nade->predAlways);
			ImGui::ColorPicker("Nade pred color", &vars::misc->nade->colorPredLine);
			ImGui::ColorPicker("Nade pred color box fill", &vars::misc->nade->colorPredBoxFill);
			ImGui::ColorPicker("Nade pred color box outline", &vars::misc->nade->colorPredBox);
			ImGui::Checkbox("Nade trails", &vars::misc->nade->enabledTracer);
			ImGui::ColorPicker("Nade trail color", &vars::misc->nade->colorTracer);
			ImGui::SliderFloat("Nade trail max dist", &vars::misc->nade->tracerDist, 1.0f, 100.0f);
			ImGui::Checkbox("Nade trail warn", &vars::misc->nade->tracerWarn);
			ImGui::SameLine();
			ImGui::HelpMarker("Draws scaled circles behind you where nade is");

		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Edits", ImGui::GetContentRegionAvail());
		{
			// FIX PERFORMANCE simply only colormodulate when color pickers state is true
			ImGui::Checkbox("Modulate world", &vars::visuals->world->modulate->enabled);
			if (vars::visuals->world->modulate->enabled)
			{
				ImGui::Checkbox("Remove sky", &vars::visuals->world->sky->removeSky);
				ImGui::ColorPicker("Texture", &vars::visuals->world->modulate->texture);
				ImGui::ColorPicker("Prop", &vars::visuals->world->modulate->prop);
				ImGui::ColorPicker("Sky", &vars::visuals->world->modulate->sky);
				ImGui::SliderFloat("Shader Alpha", &vars::visuals->world->modulate->shader, 0.0f, 100.0f);
			}

			ImGui::Checkbox("##Edit molotov", &vars::visuals->world->particles->enabledMolotov);
			ImGui::SameLine();
			ImGui::ColorPicker("Molotov##edited", &vars::visuals->world->particles->colorMolotov);
			ImGui::Checkbox("##Edit blood", &vars::visuals->world->particles->enabledBlood);
			ImGui::SameLine();
			ImGui::ColorPicker("Blood##edited", &vars::visuals->world->particles->colorBlood);
			ImGui::Checkbox("##Edit smoke", &vars::visuals->world->particles->enabledSmoke);
			ImGui::SameLine();
			ImGui::ColorPicker("Smoke##edited", &vars::visuals->world->particles->colorSmoke);

			const auto& customsky = skybox::customSkyboxes;
			bool state{ false };
			state |= ImGui::Combo("Normal", &vars::visuals->world->sky->indexNormal, std::span(selections::skyboxes.data(), selections::skyboxes.size()));
			state |= ImGui::Combo("Custom##Skybox", &vars::visuals->world->sky->indexCustom, customsky);
			skybox::changedState = state;
			if (ImGui::Button("Reload Custom Skybox"))
			{
				skybox::reloadCustomSkyboxes();
			}

			ImGui::Checkbox("Fog", &vars::visuals->world->fog->enabled);
			if (vars::visuals->world->fog->enabled)
			{
				ImGui::SliderFloat("Distance##fog", &vars::visuals->world->fog->distance, 1.0f, 1000.0f);
				ImGui::SameLine();
				ImGui::ColorPicker("Color##fog", &vars::visuals->world->fog->color);
			}

			ImGui::Checkbox("Tone", &vars::visuals->world->tone->enabled);
			if (vars::visuals->world->tone->enabled)
			{
				ImGui::SliderFloat("Min##ton", &vars::visuals->world->tone->min, 0.0f, 1.0f);
				ImGui::SliderFloat("Max##ton", &vars::visuals->world->tone->max, 0.0f, 1.0f);
				ImGui::SliderFloat("Bloom##ton", &vars::visuals->world->tone->bloom, 0.0f, 16.0f);
			}

			ImGui::Checkbox("Weather", &vars::visuals->world->weather->enabled);
			if (vars::visuals->world->weather->enabled)
			{
				weatherController::implMenu();
			}

			bool changedButton{ false };
			changedButton |= ImGui::Checkbox("##Ambientenab", &vars::visuals->world->ambient->enabled);
			ambientLight::buttonState = changedButton;

			ImGui::SameLine();
			bool changedPicker{ false };
			changedPicker |= ImGui::ColorPicker("Ambient", &vars::visuals->world->ambient->color);
			ambientLight::pickerState = changedPicker;

			ImGui::Combo("Screen effect", &vars::visuals->world->screenEffect->index, selections::screenEffects);
			ImGui::SliderFloat("Param##Screen effect", &vars::visuals->world->screenEffect->param, 0.0f, 1.0f);
			ImGui::SameLine();
			ImGui::ColorPicker("Color##Screen effect", &vars::visuals->world->screenEffect->color);

		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Hitmarker", ImGui::GetContentRegionAvail());
		{
			ImGui::Checkbox("Hitmarker", &vars::misc->hitmarker->enabled);
			ImGui::SameLine();
			ImGui::Checkbox("3D##Hitm", &vars::misc->hitmarker->enabled3D);
			ImGui::SameLine();
			ImGui::Checkbox("Resize##Hitm", &vars::misc->hitmarker->enabledResize);
			ImGui::ColorPicker("Hitmarker normal", &vars::misc->hitmarker->colorNormal);
			ImGui::ColorPicker("Hitmarker hs", &vars::misc->hitmarker->colorHead);
			ImGui::SameLine();
			ImGui::Text("Hitmarker hs");
			ImGui::ColorPicker("Hitmarker dead", &vars::misc->hitmarker->colorDead);
			ImGui::SliderFloat("Hitmarker time", &vars::misc->hitmarker->time, 0.0f, 2.0f);
			ImGui::Checkbox("Play hitmarker", &vars::misc->hitmarker->play);
			ImGui::SameLine();
			ImGui::HelpMarker("Will play the sound after every kill");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Bullets and trails", ImGui::GetContentRegionAvail());
		{
			constexpr auto flagList = "Flags list\n"
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
				"FBEAM_REVERSED 65536\n";

			ImGui::Checkbox("Movement trail", &vars::misc->trail->enabled);
			ImGui::Combo("Trail type", &vars::misc->trail->mode, magic_enum::enum_names_pretty<MovementTrail>());
			switch (vars::misc->trail->mode)
			{
			case E2T(MovementTrail::BEAM):
			{
				ImGui::SameLine();
				ImGui::PopupButton("Options##movement", []()
					{

						ImGui::Combo("Beam sprite", &vars::misc->trail->beam.index, selections::beamNames);
						ImGui::InputTextWithHint("Beam flags", "eg: 4|8, spaces allowed", &vars::misc->trail->beam.flags);
						ImGui::SameLine();
						ImGui::HelpMarker(flagList);
						ImGui::ColorPicker("Tracers color", &vars::misc->trail->beam.color);
						ImGui::SliderFloat("Tracers life", &vars::misc->trail->beam.life, 0.0f, 10.0f);
						ImGui::SliderFloat("Tracers width", &vars::misc->trail->beam.width, 0.0f, 20.0f);
						ImGui::SliderFloat("Tracers fadeLenght", &vars::misc->trail->beam.fadeLength, 0.0f, 20.0f);
						ImGui::SliderFloat("Tracers amplitude", &vars::misc->trail->beam.amplitude, 0.0f, 20.0f);
						ImGui::SliderFloat("Tracers speed", &vars::misc->trail->beam.speed, 0.0f, 200.0f);
						ImGui::SliderFloat("Tracers startFrame", &vars::misc->trail->beam.startFrame, 0.0f, 300.0f);
						ImGui::SliderFloat("Tracers frameRate", &vars::misc->trail->beam.frameRate, 0.0f, 200.0f);
						ImGui::SliderInt("Tracers segmnets", &vars::misc->trail->beam.segments, 0, 30);
					});

				break;
			}
			case E2T(MovementTrail::LINE):
			{
				ImGui::SliderFloat("Life", &vars::misc->trail->lineLife, 0.0f, 10.0f);
				ImGui::SameLine();
				ImGui::ColorPicker("Color", &vars::misc->trail->colorLine);
				break;
			}
			case E2T(MovementTrail::SPLASH):
				ImGui::SameLine();
				ImGui::ColorPicker("Color", &vars::misc->trail->colorSplash);
				break;
			}

			ImGui::Checkbox("Enable tracers", &vars::visuals->world->tracer->enabled);
			ImGui::SameLine();
			ImGui::PopupButton("Options##tracbeam", []()
				{
					ImGui::Combo("Beam sprite", &vars::visuals->world->tracer->beamTracer.index, selections::beamNames);
					ImGui::InputTextWithHint("Beam flags", "eg: 4|8, spaces allowed", &vars::visuals->world->tracer->beamTracer.flags);
					ImGui::SameLine();
					ImGui::HelpMarker(flagList);
					ImGui::ColorPicker("Tracers color", &vars::visuals->world->tracer->beamTracer.color);
					ImGui::SliderFloat("Tracers life", &vars::visuals->world->tracer->beamTracer.life, 0.0f, 10.0f);
					ImGui::SliderFloat("Tracers width", &vars::visuals->world->tracer->beamTracer.width, 0.0f, 20.0f);
					ImGui::SliderFloat("Tracers fadeLenght", &vars::visuals->world->tracer->beamTracer.fadeLength, 0.0f, 20.0f);
					ImGui::SliderFloat("Tracers amplitude", &vars::visuals->world->tracer->beamTracer.amplitude, 0.0f, 20.0f);
					ImGui::SliderFloat("Tracers speed", &vars::visuals->world->tracer->beamTracer.speed, 0.0f, 200.0f);
					ImGui::SliderFloat("Tracers startFrame", &vars::visuals->world->tracer->beamTracer.startFrame, 0.0f, 300.0f);
					ImGui::SliderFloat("Tracers frameRate", &vars::visuals->world->tracer->beamTracer.frameRate, 0.0f, 200.0f);
					ImGui::SliderInt("Tracers segmnets", &vars::visuals->world->tracer->beamTracer.segments, 0, 30);
				});

			ImGui::Checkbox("Client bullets", &vars::visuals->world->impacts->enabledClient);
			ImGui::SameLine();
			ImGui::ColorPicker("Outline##Client bullets", &vars::visuals->world->impacts->colorClient);
			ImGui::SameLine();
			ImGui::ColorPicker("Fill##Client bullets", &vars::visuals->world->impacts->colorClientFill);
			ImGui::SliderFloat("Time##Client bullets", &vars::visuals->world->impacts->timeClient, 0.0f, 5.0f);
		}
		ImGui::EndGroupPanel();
	}

	ImGui::EndChild();
	ImGui::Columns();
}