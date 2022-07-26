#include "misc.hpp"

#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/Input.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IGameEvent.hpp"
#include "../../../SDK/IWeapon.hpp"
#include "../../../SDK/IViewRenderBeams.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../SDK/CFlashlightEffect.hpp"

#include "../../../config/vars.hpp"
#include "../../game.hpp"
#include "../../globals.hpp"
#include "../../../utilities/math/math.hpp"
#include "../../../utilities/renderer/renderer.hpp"

#include "../aimbot/aimbot.hpp"

void Misc::thirdperson()
{
	if (!config.get<bool>(vars.bThirdp))
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	// TODO: reduce calls
	if (bool state = config.get<Key>(vars.kThirdp).isEnabled(); state)
	{
		Vector angles;
		interfaces::engine->getViewAngles(angles);

		Trace_t trace;

		float fixedX = angles.x += config.get<float>(vars.fThirdpX);
		float fixedY = angles.y += config.get<float>(vars.fThirdpY);

		Vector camera =
		{
			std::cos(DEG2RAD(fixedX)) * config.get<float>(vars.fThirdpDistance),
			std::sin(DEG2RAD(fixedY)) * config.get<float>(vars.fThirdpDistance),
			std::sin(DEG2RAD(-fixedX)) * config.get<float>(vars.fThirdpDistance),
		};
		Vector eyePos = game::localPlayer->getEyePos();
		TraceFilter filter;
		filter.m_skip = game::localPlayer();
		interfaces::trace->traceRay({ eyePos, (eyePos - camera) }, MASK_SOLID, &filter, &trace);

		interfaces::input->m_cameraInThirdPerson = state;
		interfaces::input->m_cameraOffset = Vector{ fixedX, fixedY, config.get<float>(vars.fThirdpDistance) * ((trace.m_fraction < 1.0f) ? trace.m_fraction : 1.0f) };
	}
	else if(globals::isShutdown || !state)
	{
		interfaces::input->m_cameraInThirdPerson = state;
		interfaces::input->m_cameraOffset = {};
	}
}

void Misc::drawCrosshair()
{
	int cfgCross = config.get<int>(vars.iCrosshair);

	const static auto cl_crosshair_recoil = interfaces::cvar->findVar(XOR("cl_crosshair_recoil"));
	cl_crosshair_recoil->setValue(cfgCross == E2T(CrossHairTypes::ENGINE) ? true : false);

	if (!cfgCross)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isConnected())
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x = globals::screenX;
	int y = globals::screenY;
	x /= 2.0f;
	y /= 2.0f;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	auto getPunchPos = [=]()
	{
		Vector angle;
		interfaces::engine->getViewAngles(angle);
		const static auto scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"))->getFloat();
		angle += game::localPlayer->m_aimPunchAngle() * scale;

		auto start = game::localPlayer->getEyePos();
		auto end = start + math::angleVec(angle) * weapon->getWpnInfo()->m_range;

		return end;
	};

	switch (cfgCross)
	{
	case E2T(CrossHairTypes::STATIC):
	{
		float moveCross = 8.0f;

		imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::Black, 3.0f);
		imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::Black, 3.0f);

		moveCross -= 1.5f;

		imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::LightBlue, 1.0f);
		imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::LightBlue, 1.0f);
		break;
	}
	case E2T(CrossHairTypes::RECOIL):
	{
		if (Vector2D endScreen; imRender.worldToScreen(getPunchPos(), endScreen))
		{
			int x = endScreen.x;
			int y = endScreen.y;

			float moveCross = 8.0f;

			imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::Black, 3.0f);
			imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::Black, 3.0f);

			moveCross -= 1.5f;

			imRender.drawLine(x - moveCross, y, x + moveCross, y, Colors::LightBlue, 1.0f);
			imRender.drawLine(x, y - moveCross, x, y + moveCross, Colors::LightBlue, 1.0f);
		}
		break;
	}
	case E2T(CrossHairTypes::SPREAD):
	{
		if (Vector2D endScreen; imRender.worldToScreen(getPunchPos(), endScreen))
		{
			// this is game's logic how to do it
			/*float spread = weapon->getSpread();
			float inaccuracy = weapon->getInaccuracy();
			float scaledSpread = ((inaccuracy + spread) * 320.0f / std::tan(DEG2RAD(globals::FOV) / 2.0f));
			int radiusSpread = scaledSpread * y / 480.0f;

			if (game::localPlayer->m_vecVelocity().length2D() > 0.0f)
				radiusSpread = inaccuracy * 1000.0f;*/
			float inaccuracy = weapon->getInaccuracy();
			int radiusSpread = inaccuracy * 1000.0f;
			int x = endScreen.x;
			int y = endScreen.y;

			imRender.drawCircle(x, y, radiusSpread, 32, Colors::Black);
			imRender.drawCircleFilled(x, y, radiusSpread, 32, Colors::LightBlue.getColorEditAlpha(0.2f));
		}
		break;
	}
	default:
		break;
	}
}

void Misc::drawLocalInfo()
{
	if (!config.get<bool>(vars.bDrawMiscInfo))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	int width = globals::screenX * 0.6f;

	imRender.text(width, 15, ImFonts::tahoma14, FORMAT(XOR("Map: {}"), interfaces::engine->getLevelName()), false, Colors::Green);
	imRender.text(width, 25, ImFonts::tahoma14, FORMAT(XOR("Weapon {} [{} / {}]"), weapon->getWpnInfo()->m_WeaponName, weapon->m_iClip1(), weapon->m_iPrimaryReserveAmmoCount()), false, Colors::Yellow);
	imRender.text(width, 35, ImFonts::tahoma14, FORMAT(XOR("Current In-accuracy {:.2f}%"), weapon->getInaccuracy() * 100), false, Colors::Yellow);
	imRender.text(width, 45, ImFonts::tahoma14, FORMAT(XOR("Zoom level {}"), weapon->m_zoomLevel()), false, Colors::Yellow);
	imRender.text(width, 55, ImFonts::tahoma14, FORMAT(XOR("POS: x {:.2f} y {:.2f} z {:.2f}"), game::localPlayer->absOrigin().x, game::localPlayer->absOrigin().y, game::localPlayer->absOrigin().z), false, Colors::Yellow);
	imRender.text(width, 65, ImFonts::tahoma14, FORMAT(XOR("Velocity {:.2f}"), game::localPlayer->m_vecVelocity().length2D()), false, Colors::Yellow);

	imRender.text(width, 75, ImFonts::tahoma14, FORMAT(XOR("Kills {}"), game::localPlayer->getKills()), false, Colors::Yellow);
	imRender.text(width, 85, ImFonts::tahoma14, FORMAT(XOR("Deaths {}"), game::localPlayer->getDeaths()), false, Colors::Yellow);
	// escape divide by zero exceptions by using this trick
	float kd = game::localPlayer->getKills() / (game::localPlayer->getDeaths() ? game::localPlayer->getDeaths() : 1.0f);
	float kpm = game::localPlayer->getKills() / (game::serverTime() / 60.0f);
	imRender.text(width, 95, ImFonts::tahoma14, FORMAT(XOR("KD {:.2f} KPM: {:.2f}"), kd, kpm), false, Colors::Yellow);
	imRender.text(width, 105, ImFonts::tahoma14, FORMAT(XOR("Ping {}"), game::localPlayer->getPing()), false, Colors::Yellow);

	float accuracy = globals::shotsFired
		? (static_cast<float>(globals::shotsHit) / static_cast<float>(globals::shotsFired)) * 100.0f
		: 0.0f;
	float fixedKills = game::localPlayer->getKills() ? game::localPlayer->getKills() : 1.0f;
	float hs = globals::shotsHead
		? (static_cast<float>(globals::shotsHead) / fixedKills) * 100.0f
		: 0.0f;
	imRender.text(width, 115, ImFonts::tahoma14, FORMAT(XOR("Accuracy [{} / {}] {:.2f}% HS {:.2f}%"), globals::shotsHit, globals::shotsFired, accuracy, hs), false, Colors::Yellow);

	width *= 1.25f;
	imRender.text(width, 15, ImFonts::tahoma14, aimbot.getTargetted() ? FORMAT(XOR("Aimbot working on: {}"), aimbot.getTargetted()->getName()) : "", false, Colors::LightBlue);
}

#include "../../../utilities/console/console.hpp"
#include "../../menu/GUI-ImGui/imguiaddons.hpp"
#include <numeric>

void Misc::drawFpsPlot()
{
	bool& plotRef = config.getRef<bool>(vars.bShowFpsPlot);
	if (!plotRef)
		return;

	// static so we can get records get saved
	static std::deque<float> records;

	auto getfps = []() -> float // this is how they do it
	{
		static float realfps = 0.0f;
		realfps = 0.9f * realfps + (1.0f - 0.9f) * interfaces::globalVars->m_absoluteframetime;

		return 1.0f / realfps;
	};

	const float fps = getfps();
	// when loading, you have a chance of freezing
	if (std::isinf(fps))
	{
		console.log(TypeLogs::LOG_WARN, XOR("record for plot got skipped due to infinity"));
		return;
	}

	constexpr size_t RECORDS_SIZE = 300;

	static float acceptanceCustom = 1.0f;

	records.push_back(fps);

	while (records.size() > static_cast<size_t>(RECORDS_SIZE / acceptanceCustom))
		records.pop_front();

	static float MAX_FPS = interfaces::engine->isInGame() ? 350.0f : 120.0f;

	bool& sliderRef = config.getRef<bool>(vars.bFPSCustom);
	if (sliderRef)
	{
		if (ImGui::Begin(XOR("Sliders for plot"), &sliderRef, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (interfaces::engine->isInGame()) // not enaled in menu
				ImGui::SliderFloat(XOR("Set max FPS"), &MAX_FPS, 100.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // hardcoded ranges, we should maybe run some avg fps getter
			ImGui::SliderFloat(XOR("Acceptance multiply fps"), &acceptanceCustom, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::ColorPicker(XOR("Color lines plot fps"), &config.getRef<CfgColor>(vars.cFps));

			ImGui::End();
		}
	}

	if (ImGui::Begin(FORMAT(XOR("Plot FPS AVG {:.2f}###Plot FPS"), std::reduce(records.begin(), records.end()) / records.size()).c_str(), &plotRef, ImGuiWindowFlags_NoCollapse))
	{
		imRenderWindow.addList(); // correct pos, so we start from x = 0 y = 0
		float acceptance = imRenderWindow.getWidth() / static_cast<float>(RECORDS_SIZE / acceptanceCustom);
		float prevX = 0.0f, prevY = 0.0f;
		float scaledavg = MAX_FPS / imRenderWindow.getHeight(); // might run some better logic...
		for (size_t i = 0; const auto & currentFPS : records)
		{
			float currentX = i * acceptance; // no need for clamping

			float currentY = imRenderWindow.getHeight() - (currentFPS / scaledavg); // is there any better way for this?
			if (currentY < 1.0f)
				currentY = 1.0f; // stay on top to clamp

			// start drawing after there is any change, we could use polylines here
			if (i > 0)
				imRenderWindow.drawLine(currentX, currentY, prevX, prevY, config.get<CfgColor>(vars.cFps).getColor());
			prevX = currentX; prevY = currentY;

			i++;
		}

		imRenderWindow.end();

		ImGui::End();
	}
	if (!plotRef)
		sliderRef = false;
}

// because without prediction the values might not be that accurate
void Misc::getVelocityData()
{
	if (!config.get<bool>(vars.bShowVelocityPlot))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame() || !game::localPlayer->isAlive())
	{
		velRecords.clear();
		return;
	}

	float vel = game::localPlayer->m_vecVelocity().length2D();
	if (std::isinf(vel))
	{
		console.log(TypeLogs::LOG_WARN, XOR("record for plot got skipped due to infinity"));
		return;
	}

	velRecords.emplace_back(game::localPlayer->m_vecVelocity().length2D());

	// width
	while (velRecords.size() > static_cast<size_t>(RECORDS_SIZE_VEL / acceptanceVelocityCustom))
		velRecords.pop_front();
}

void Misc::drawVelocityPlot()
{
	bool& plotVelRef = config.getRef<bool>(vars.bShowVelocityPlot);
	if (!plotVelRef)
		return;

	if (!interfaces::engine->isInGame() || !game::localPlayer->isAlive())
		return;

	static float MAX_SPEEED_MOVE = interfaces::cvar->findVar(XOR("sv_maxspeed"))->getFloat(); // should be accurate
	static bool transparent = false;

	bool& sliderRef = config.getRef<bool>(vars.bVelocityCustom);
	if (sliderRef)
	{
		if (ImGui::Begin(XOR("Sliders for velocity plot"), &sliderRef, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			//ImGui::SliderFloat(XOR("Set max velocity"), &MAX_SPEEED_MOVE, 450.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // who will ever get this velocity? adjust if needed
			ImGui::SliderFloat(XOR("Acceptance multiply vel"), &acceptanceVelocityCustom, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::ColorPicker(XOR("Color lines plot vel"), &config.getRef<CfgColor>(vars.cVelocityPlot));
			ImGui::Checkbox(XOR("Run transparent"), &transparent);
			ImGui::SameLine();
			ImGui::HelpMarker(XOR("Will add some flags!\nEg: no resize"));

			ImGui::End();
		}
	}

	int flags = transparent
		? ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		: ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin(FORMAT(XOR("Plot Velocity AVG {:.2f}###Plot Velocity"), std::reduce(velRecords.begin(), velRecords.end()) / velRecords.size()).c_str(), &plotVelRef, flags))
	{
		imRenderWindow.addList(); // correct pos, so we start from x = 0 y = 0	
		float acceptance = imRenderWindow.getWidth() / static_cast<float>(RECORDS_SIZE_VEL / acceptanceVelocityCustom);
		float prevX = 0.0f, prevY = 0.0f;
		float prevVel = 0.0f;
		float scaledavg = MAX_SPEEED_MOVE / imRenderWindow.getHeight(); // might run some better logic...
		float scaledFontSize = imRenderWindow.getWidth() / 25.0f;
		float width = imRenderWindow.getWidth();
		float height = imRenderWindow.getHeight();

		for (size_t i = 0; const auto & currentVel : velRecords)
		{
			float deltaVel = std::abs(currentVel - prevVel);

			float currentX = i * acceptance; // no need for clamping

			float currentY = imRenderWindow.getHeight() - (currentVel / scaledavg); // is there any better way for this?
			if (currentY < 1.0f)
				currentY = 1.0f; // stay on top to clamp

			// start drawing after there is any change, we could use polylines here
			if (i > 0)
			{
				imRenderWindow.drawLine(currentX, currentY, prevX, prevY, config.get<CfgColor>(vars.cVelocityPlot).getColor());
				if (deltaVel > 20) // would need proper edge detection
					imRenderWindow.drawText(static_cast<int>(currentX) + 10, static_cast<int>(currentY) - 10, scaledFontSize, ImFonts::franklinGothic30,
						std::format("{}", std::round(currentVel)), true, Colors::Pink, false);
			}
			prevX = currentX; prevY = currentY;
			prevVel = currentVel;

			i++;
		}

		auto text = [drawing = ImGui::GetBackgroundDrawList()]
		(const float x, const float y, const float size, ImFont* font, const std::string& text, const bool centered, const Color& color, const bool dropShadow)
		{
			ImVec2 pos = { x, y };

			if (auto tsize = ImGui::CalcTextSize(text.c_str()); centered)
				pos.x -= tsize.x / 2.0f;

			if (dropShadow)
			{
				const auto alpha = ImGui::ColorConvertU32ToFloat4(U32(color)).z;
				drawing->AddText(font, size, { pos.x + 1.0f, pos.y + 1.0f }, U32(Colors::Black.getColorEditAlpha(alpha)), text.c_str());
			}
			drawing->AddText(font, size, pos, U32(color), text.c_str());
		};

		text(imRenderWindow.getPos().x + width / 2.0f, imRenderWindow.getPos().y + 20.0f + height, 30.0f, ImFonts::franklinGothic30,
			std::format("{}", std::round(game::localPlayer->m_vecVelocity().length2D())), true, Colors::Pink, false);

		imRenderWindow.end();
		ImGui::End();
	}
	if (!plotVelRef)
		sliderRef = false;
}

void Misc::playHitmarker(IGameEvent* event)
{
	auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("attacker"))));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;
	
	auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!ent) // should never happen
		return;

	globals::shotsHit++;

	auto dmg_health = event->getInt(XOR("dmg_health"));
	auto health = ent->m_iHealth() - dmg_health;
	auto hitgroup = event->getInt(XOR("hitgroup"));

	if (health < 0 && hitgroup == 1)
		globals::shotsHead++;

	if (!config.get<bool>(vars.bDrawHitmarker))
		return;

	Hitmark_t hit =
	{
		interfaces::globalVars->m_curtime + config.get<float>(vars.fHitmarkerTime),
		dmg_health,
		ent->m_iHealth() - dmg_health,
		hitgroup == 1, // head
		ent->getHitgroupPos(hitgroup)
	};
	m_hitmarkers.push_back(hit);
	if (config.get<bool>(vars.bPlayHitmarker))
		interfaces::surface->playSound(XOR("buttons\\arena_switch_press_02.wav"));
}

void Misc::drawHitmarker()
{
	if (!config.get<bool>(vars.bDrawHitmarker))
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x, y;
	bool mode3D = config.get<bool>(vars.bDrawHitmarker3D);
	if (!mode3D)
	{
		x = globals::screenX / 2.0f;
		y = globals::screenY / 2.0f;
	}

	float currentAlpha = 0.0f;
	for (size_t i = 0; const auto & el : m_hitmarkers)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitmarkers.erase(m_hitmarkers.begin() + i);
			continue;
		}

		Vector2D s;
		if (mode3D)
		{
			if (!imRender.worldToScreen(el.m_pos, s))
				continue;

			x = s.x;
			y = s.y;
		}

		currentAlpha = diff / config.get<float>(vars.fHitmarkerTime);
		float sizeFont = 16.0f;
		CfgColor actualColor = config.get<CfgColor>(vars.cDrawHitmarkerNormal).getColor().getColorEditAlpha(currentAlpha);
		float lineX = 10.0f;
		float lineY = 5.0f;

		if (el.isAvailable() && el.m_head)
		{
			actualColor = config.get<CfgColor>(vars.cDrawHitmarkerHead).getColor().getColorEditAlpha(currentAlpha);
			sizeFont = 24.0f;
			lineX = 14.0f;
			lineY = 7.0f;
		}
		else if (!el.isAvailable())
		{
			actualColor = config.get<CfgColor>(vars.cDrawHitmarkerDead).getColor().getColorEditAlpha(currentAlpha);
			sizeFont = 28.0f;
			lineX = 18.0f;
			lineY = 9.0f;
		}

		float lineAddonX = lineX;
		float lineAddonY = lineY;
		if (config.get<bool>(vars.bDrawHitmarkerResize))
		{
			lineAddonX = lineX / (1.0f / (currentAlpha + 0.01f)); // prevent division by 0 and make ratio
			lineAddonY = lineY / (1.0f / (currentAlpha + 0.01f));
		}

		imRender.drawLine(x - lineAddonX, y + lineAddonX, x - lineAddonY, y + lineAddonY, actualColor.getColor());
		imRender.drawLine(x + lineAddonX, y + lineAddonX, x + lineAddonY, y + lineAddonY, actualColor.getColor());
		imRender.drawLine(x - lineAddonX, y - lineAddonX, x - lineAddonY, y - lineAddonY, actualColor.getColor());
		imRender.drawLine(x + lineAddonX, y - lineAddonX, x + lineAddonY, y - lineAddonY, actualColor.getColor());

		constexpr int moveMultiply = 25;
		float correction = (1.0f - currentAlpha) * moveMultiply; // this maybe should have el.expire ratio to previous one
		float Xcorrection = x + 8.0f + (correction * 0.6f); // multiply 0.6 to get a bit niver effect, 8 comes from padding
		float Ycorrection = y - (correction * 4.0f); // 4.0f comes from hardcoding. Make it more nice, maybe there are better ways for this

		imRender.text(Xcorrection, Ycorrection, sizeFont, ImFonts::tahoma14, FORMAT(XOR("{}"), el.m_dmg), false, actualColor.getColor(), false);

		i++;
	}
}

void Misc::drawNoScope()
{
	if (!config.get<bool>(vars.bNoScope))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (!weapon->isSniper())
		return;

	if (game::localPlayer->m_bIsScoped())
	{
		int x = globals::screenX;
		int y = globals::screenY;
		imRender.drawLine(x / 2.0f, 0.0f, x / 2.0f, y, Colors::Black);
		imRender.drawLine(0.0f, y / 2.0f, x, y / 2.0f, Colors::Black);
	}
}

// because this is special case... I had to copy this with few edits
static void drawConeEditedRainbow(const Vector& pos, const float radius, const int points, const float size, const float speed, const int trianglesAlpha, const int linesAlpha, const float thickness = 2.0f)
{
	ImVec2 orignalW2S = {};
	if (!imRender.worldToScreen(pos, orignalW2S))
		return;

	float step = std::numbers::pi_v<float> *2.0f / points;
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector startWorld = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector endWorld = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (ImVec2 start, end; imRender.worldToScreen(startWorld, start) && imRender.worldToScreen(endWorld, end))
		{
			imRender.drawLine(start, end, Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(linesAlpha), thickness);
			imRender.drawTrianglePoly({ orignalW2S.x, orignalW2S.y + size }, start, end, Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(trianglesAlpha));
		}
	}
}

// more points - more clean
void Misc::drawHat()
{
	if (!config.get<bool>(vars.bHat))
		return;

	if (!game::isAvailable())
		return;

	if (!interfaces::input->m_cameraInThirdPerson)
		return;

	auto pos = game::localPlayer->getBonePos(8); // you can play around with vec.z

	// config.get<> enjoyer

	if (config.get<bool>(vars.bHatRainbow))
	{
		drawConeEditedRainbow(pos, config.get<float>(vars.fHatRadius), 86, config.get<float>(vars.fHatSize), config.get<float>(vars.fHatSpeed), config.get<int>(vars.iHatTriangleAlpha), config.get<int>(vars.iHatLinesAlpha));
	}
	else
	{
		imRender.drawCone(pos, config.get<float>(vars.fHatRadius), 86, config.get<float>(vars.fHatSize),
			config.get<CfgColor>(vars.cHatTriangle).getColor(), config.get<CfgColor>(vars.cHatLine).getColor(), true, 2.0f);
	}
}

void Misc::drawBulletTracer(const Vector& start, const Vector& end)
{
	if (!config.get<bool>(vars.bDrawBulletTracer))
		return;

	CfgColor color = config.get<CfgColor>(vars.cDrawBulletTracer);

	Trace_t tr;
	TraceFilter filter;
	filter.m_skip = game::localPlayer();
	interfaces::trace->traceRay({ start, end }, MASK_PLAYER, &filter, &tr);

	BeamInfo_t info = {};

	info.m_type = TE_BEAMPOINTS;
	info.m_flags = FBEAM_FADEIN | FBEAM_FADEOUT;
	info.m_modelName = XOR("sprites/physbeam.vmt");
	info.m_modelIndex = -1;
	info.m_haloIndex = -1;
	info.m_haloScale = 3.0f;
	info.m_life = config.get<float>(vars.fDrawBulletTracer);
	info.m_width = 2.0f;
	info.m_endWidth = 2.0f;
	info.m_fadeLength = 1.0f;
	info.m_amplitude = 2.0f;
	info.m_red = color.getColor().rMultiplied();
	info.m_green = color.getColor().gMultiplied();
	info.m_blue = color.getColor().bMultiplied();
	info.m_brightness = color.getColor().aMultiplied();
	info.m_speed = 0.2f;
	info.m_startFrame = 0.0f;
	info.m_frameRate = 60.0f;
	info.m_vecStart = start;
	info.m_vecEnd = tr.m_end;
	info.m_segments = 2;
	info.m_renderable = true;

	interfaces::beams->createBeamPoints(info);
}

#include "../../../SDK/CFlashlightEffect.hpp"
#include "../../../SDK/IMemAlloc.hpp"

static CFlashlightEffect* createFlashlight(float fov, Entity_t* ent, const char* effectName = XOR("effects/flashlight001"), float farZ = 1000.0f, float linearAtten = 1000.0f)
{
	auto flashlightMemory = reinterpret_cast<CFlashlightEffect*>(interfaces::memAlloc->_alloc(sizeof(CFlashlightEffect)));
	if (!flashlightMemory)
		return nullptr;

	//__userpurge, this works which I am surprised, but I don't want to give bad example
	//using fn = CFlashlightEffect * (__thiscall*)(void*, void*, float, float, float, float, int, const char*, float, float);
	const static auto create = utilities::patternScan(CLIENT_DLL, FLASHLIGHT_CREATE);

	int idx = ent->getIndex(); // allow asm passing this arg
	__asm
	{
		mov eax, fov // fov = (*(int (__stdcall **)(const char *, const char *, signed int, signed int))(*(_DWORD *)ptrfl + 0x16C))
		mov ecx, flashlightMemory
		push linearAtten
		push farZ
		push effectName
		push idx
		call create
		// crash, tried to wrap __userpurge
		//leave // for userpurge
		//ret // for userpurge
	}

	//create(flashlightMemory, nullptr, 0.0f, 0.0f, 0.0f, fov, ent->getIndex(), pszTextureName, farZ, linearAtten);

	return flashlightMemory;
}

static void destroyFlashLight(CFlashlightEffect* flashlight)
{
	using fn = void(__thiscall*)(void*, void*);
	const static auto destroy = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, FLASHLIGHT_DESTROY));
	destroy(flashlight, nullptr); // second arg is not even used there
}

static void updateFlashlight(CFlashlightEffect* flashlight, const Vector& pos, const Vector& forward, const Vector& right, const Vector& up)
{
	using fn = void(__thiscall*)(void*, int, const Vector&, const Vector&, const Vector&, const Vector&, float, float, float, bool, const char*);
	const static uintptr_t update = utilities::patternScan(CLIENT_DLL, FLASHLIGHT_UPDATE); // call
	const static uintptr_t relativeAddress = *reinterpret_cast<uintptr_t*>(update + 0x1); // read the rel32
	const static uintptr_t nextInstruction = update + 0x5; // mov eax, [esi]

	const static auto fun = reinterpret_cast<fn>(relativeAddress + nextInstruction);
	fun(flashlight, flashlight->m_entIndex, pos, forward, right, up, flashlight->m_fov, flashlight->m_farZ, flashlight->m_LinearAtten, flashlight->m_castsShadows, flashlight->m_textureName);
}

void Misc::flashLight(int frame)
{
	if (m_flashLight && globals::isShutdown)
	{
		static auto bOnce = [=]()
		{
			destroyFlashLight(m_flashLight);
			m_flashLight = nullptr;

			return true;
		} ();
	}

	if (globals::isShutdown)
		return;

	if (frame != FRAME_RENDER_START)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	if (!config.get<bool>(vars.bFlashlight))
		return;

	auto key = config.get<Key>(vars.kFlashlight);
	switch (key.getKeyMode())
	{
	case KeyMode::DOWN:
	{
		static bool done = false;

		if (key.isDown())
		{
			if (!done)
			{
				interfaces::surface->playSound(XOR("items\\flashlight1.wav"));
				m_flashLight = createFlashlight(config.get<float>(vars.fFlashlightFov), game::localPlayer());
				done = true;
			}
		}
		else
		{
			if (done)
			{
				destroyFlashLight(m_flashLight);
				m_flashLight = nullptr;
				done = false;
			}
		}

		break;
	}
	case KeyMode::TOGGLE:
	{
		if (key.isPressed())
		{
			interfaces::surface->playSound(XOR("items\\flashlight1.wav"));

			if (m_flashLight)
			{
				destroyFlashLight(m_flashLight);
				m_flashLight = nullptr;
			}
			else
				m_flashLight = createFlashlight(config.get<float>(vars.fFlashlightFov), game::localPlayer());
		}

		break;
	}
	}

	if (!m_flashLight)
		return;

	Vector forward, right, up;
	Vector angle;
	interfaces::engine->getViewAngles(angle);
	math::angleVectors(angle, forward, right, up);

	m_flashLight->m_isOn = true;
	m_flashLight->m_castsShadows = true;
	m_flashLight->m_bigMode = config.get<bool>(vars.bFlashlightBigMode);
	m_flashLight->m_fov = config.get<float>(vars.fFlashlightFov);

	updateFlashlight(m_flashLight, game::localPlayer->getEyePos(), forward, right, up);
}