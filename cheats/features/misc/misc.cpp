#include "misc.hpp"

#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/Input.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IGameEvent.hpp"
#include "../../../SDK/IWeapon.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

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

	constexpr int VK_VKEY = 0x56; // this is only for test now
	static bool thirdp = false;
	if (utilities::getKey(VK_VKEY) & 1)
		thirdp = !thirdp;

	interfaces::input->m_cameraInThirdPerson = thirdp;
	interfaces::input->m_cameraOffset.z = 220.0f;
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

	imRender.text(width, 15, ImFonts::tahoma, std::format(XOR("Map: {}"), interfaces::engine->getLevelName()), false, Colors::Green);
	imRender.text(width, 25, ImFonts::tahoma, std::format(XOR("Weapon {} [{} / {}]"), weapon->getWpnName(), weapon->m_iClip1(), weapon->m_iPrimaryReserveAmmoCount()), false, Colors::Yellow);
	imRender.text(width, 35, ImFonts::tahoma, std::format(XOR("Current In-accuracy {:.2f}%"), weapon->getInaccuracy() * 100), false, Colors::Yellow);
	imRender.text(width, 45, ImFonts::tahoma, std::format(XOR("Zoom level {}"), weapon->m_zoomLevel()), false, Colors::Yellow);
	imRender.text(width, 55, ImFonts::tahoma, std::format(XOR("POS: x {:.2f} y {:.2f} z {:.2f}"), game::localPlayer->absOrigin().x, game::localPlayer->absOrigin().y, game::localPlayer->absOrigin().z), false, Colors::Yellow);
	imRender.text(width, 65, ImFonts::tahoma, std::format(XOR("Velocity {:.2f}"), game::localPlayer->m_vecVelocity().length2D()), false, Colors::Yellow);

	imRender.text(width, 75, ImFonts::tahoma, std::format(XOR("Kills {}"), game::localPlayer->getKills()), false, Colors::Yellow);
	imRender.text(width, 85, ImFonts::tahoma, std::format(XOR("Deaths {}"), game::localPlayer->getDeaths()), false, Colors::Yellow);
	// escape divide by zero exceptions by using this trick
	float kd = game::localPlayer->getKills() / (game::localPlayer->getDeaths() ? game::localPlayer->getDeaths() : 1.0f);
	imRender.text(width, 95, ImFonts::tahoma, std::format(XOR("KD {:.2f}"), kd), false, Colors::Yellow);
	imRender.text(width, 105, ImFonts::tahoma, std::format(XOR("Ping {}"), game::localPlayer->getPing()), false, Colors::Yellow);

	float accuracy = globals::shotsFired
		? (static_cast<float>(globals::shotsHit) / static_cast<float>(globals::shotsFired)) * 100.0f
		: 0.0f;
	float fixedKills = game::localPlayer->getKills() ? game::localPlayer->getKills() : 1.0f;
	float hs = globals::shotsHead
		? (static_cast<float>(globals::shotsHead) / fixedKills) * 100.0f
		: 0.0f;
	imRender.text(width, 115, ImFonts::tahoma, std::format(XOR("Accuracy [{} / {}] {:.2f}% HS {:.2f}%"), globals::shotsHit, globals::shotsFired, accuracy, hs), false, Colors::Yellow);

	width *= 1.25f;
	imRender.text(width, 15, ImFonts::tahoma, aimbot.getTargetted() ? std::format(XOR("Aimbot working on: {}"), aimbot.getTargetted()->getName()) : "", false, Colors::LightBlue);
}

#include "../../../utilities/console/console.hpp"
#include "../../menu/GUI-ImGui/imguiaddons.hpp"

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

	static float MAX_FPS = interfaces::engine->isInGame() ? 350.0f : 120.0f; // fps are limied in menu, then in game use whatever u want
	static Color cLine = Colors::White;

	bool& sliderRef = config.getRef<bool>(vars.bFPSCustom);
	// i should fix this
	if (sliderRef)
	{
		if (ImGui::Begin(XOR("Sliders for plot"), &sliderRef, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (interfaces::engine->isInGame()) // not enaled in menu
				ImGui::SliderFloat(XOR("Set max FPS"), &MAX_FPS, 100.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // hardcoded ranges, we should maybe run some avg fps getter
			ImGui::SliderFloat(XOR("Acceptance multiply"), &acceptanceCustom, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::ColorPicker(XOR("Color lines plot"), &cLine);
			ImGui::SameLine();
			ImGui::Text(XOR("Color line"));

			ImGui::End();
		}
	}

	if (ImGui::Begin(XOR("Plot FPS"), &plotRef, ImGuiWindowFlags_NoCollapse))
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
				imRenderWindow.drawLine(currentX, currentY, prevX, prevY, cLine);
			prevX = currentX; prevY = currentY;

			i++;
		}

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
	static Color cLine = Colors::White;

	bool& sliderRef = config.getRef<bool>(vars.bVelocityCustom);
	if (sliderRef)
	{
		if (ImGui::Begin(XOR("Sliders for velocity plot"), &sliderRef, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			//ImGui::SliderFloat(XOR("Set max velocity"), &MAX_SPEEED_MOVE, 450.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // who will ever get this velocity? adjust if needed
			ImGui::SliderFloat(XOR("Acceptance multiply##vel"), &acceptanceVelocityCustom, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::ColorPicker(XOR("Color lines plot##vel"), &cLine);
			ImGui::SameLine();
			ImGui::Text(XOR("Color line"));

			ImGui::End();
		}
	}

	if (ImGui::Begin(XOR("Plot Velocity"), &plotVelRef, ImGuiWindowFlags_NoCollapse))
	{
		imRenderWindow.addList(); // correct pos, so we start from x = 0 y = 0
		float acceptance = imRenderWindow.getWidth() / static_cast<float>(RECORDS_SIZE_VEL / acceptanceVelocityCustom);
		float prevX = 0.0f, prevY = 0.0f;
		float scaledavg = MAX_SPEEED_MOVE / imRenderWindow.getHeight(); // might run some better logic...
		for (size_t i = 0; const auto & currentVel : velRecords)
		{
			float currentX = i * acceptance; // no need for clamping

			float currentY = imRenderWindow.getHeight() - (currentVel / scaledavg); // is there any better way for this?
			if (currentY < 1.0f)
				currentY = 1.0f; // stay on top to clamp

			// start drawing after there is any change, we could use polylines here
			if (i > 0)
				imRenderWindow.drawLine(currentX, currentY, prevX, prevY, cLine);
			prevX = currentX; prevY = currentY;

			i++;
		}

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
		hitgroup == 1 // head
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

	int x = globals::screenX;
	int y = globals::screenY;
	x /= 2.0f, y /= 2.0f;

	float currentAlpha = 0.0f;
	Color actualColor;
	for (size_t i = 0; const auto & el : m_hitmarkers)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitmarkers.erase(m_hitmarkers.begin() + i);
			continue;
		}

		currentAlpha = diff / config.get<float>(vars.fHitmarkerTime);

		if (el.isAvailable() && el.m_head)
			actualColor = Colors::Pink.getColorEditAlpha(currentAlpha);
		else if (!el.isAvailable())
			actualColor = Colors::Green.getColorEditAlpha(currentAlpha);
		else
			actualColor = Colors::White.getColorEditAlpha(currentAlpha);

		constexpr int moveMultiply = 25;
		float correction = (1.0f - currentAlpha) * moveMultiply; // this maybe should have el.expire ratio to previus one
		float Xcorrection = x + 8.0f + (correction * 0.6f); // multiply 0.6 to get a bit niver effect, 8 comes from padding
		float Ycorrection = y - (correction * 4.0f); // 4.0f comes from hardcoding. Make it more nice, maybe there are better ways for this

		imRender.text(Xcorrection, Ycorrection, ImFonts::tahoma, std::format(XOR("{}"), el.m_dmg), false, actualColor);

		i++;
	}

	constexpr float lineX = 10.0f;
	constexpr float lineY = 5.0f;
	float lineAddonX = lineX / (1.0f / (currentAlpha + 0.01f)); // prevent division by 0 and make ratio
	float lineAddonY = lineY / (1.0f / (currentAlpha + 0.01f));

	if (!m_hitmarkers.empty())
	{
		imRender.drawLine(x - lineAddonX, y + lineAddonX, x - lineAddonY, y + lineAddonY, actualColor);
		imRender.drawLine(x + lineAddonX, y + lineAddonX, x + lineAddonY, y + lineAddonY, actualColor);
		imRender.drawLine(x - lineAddonX, y - lineAddonX, x - lineAddonY, y - lineAddonY, actualColor);
		imRender.drawLine(x + lineAddonX, y - lineAddonX, x + lineAddonY, y - lineAddonY, actualColor);
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
	Vector2D orignalW2S = {};
	if (!imRender.worldToScreen(pos, orignalW2S))
		return;

	float step = std::numbers::pi_v<float> *2.0f / points;
	for (float angle = 0.0f; angle < (std::numbers::pi_v<float> *2.0f); angle += step)
	{
		Vector startWorld = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector endWorld = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (Vector2D start, end; imRender.worldToScreen(startWorld, start) && imRender.worldToScreen(endWorld, end))
		{
			imRender.drawLine(start, end, Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(linesAlpha), thickness);
			surfaceRender.drawTriangleFilled({ orignalW2S.x, orignalW2S.y + size }, start, end, Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(trianglesAlpha));
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
		imRender.drawCone(pos, config.get<float>(vars.fHatRadius), 86, config.get<float>(vars.fHatSize), config.get<Color>(vars.cHatTriangle), config.get<Color>(vars.cHatLine), true, 2.0f);
	}
}
