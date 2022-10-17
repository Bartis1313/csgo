#include "plots.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <config/vars.hpp>
#include <utilities/utilities.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <numeric>

void Plots::draw()
{
	drawFps();
	drawVelocity();
}

void Plots::drawFps()
{
	if (!vars::misc->plots->enabledFps)
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
		LOG_WARN("record for plot got skipped due to infinity");
		return;
	}

	static float acceptanceCustom = 1.0f;

	records.push_back(fps);

	while (records.size() > static_cast<size_t>(RECORDS_SIZE / acceptanceCustom))
		records.pop_front();

	static float MAX_FPS = interfaces::engine->isInGame() ? 350.0f : 120.0f;

	if (vars::misc->plots->fpsCustom)
	{
		if (ImGui::Begin(XOR("Sliders for plot"), &vars::misc->plots->fpsCustom, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (interfaces::engine->isInGame()) // not enaled in menu
				ImGui::SliderFloat(XOR("Set max FPS"), &MAX_FPS, 30.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // hardcoded ranges, we should maybe run some avg fps getter
			ImGui::SliderFloat(XOR("Acceptance multiply fps"), &acceptanceCustom, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::ColorPicker(XOR("Color lines plot fps"), &vars::misc->plots->colorFPS);

			ImGui::End();
		}
	}

	if (ImGui::Begin(FORMAT(XOR("Plot FPS AVG {:.2f}###Plot FPS"),
		std::reduce(records.begin(), records.end()) / records.size()).c_str(), &vars::misc->plots->enabledFps, ImGuiWindowFlags_NoCollapse))
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
				imRenderWindow.drawLine(currentX, currentY, prevX, prevY, vars::misc->plots->colorFPS());
			prevX = currentX; prevY = currentY;

			i++;
		}

		imRenderWindow.end();

		ImGui::End();
	}
}

void Plots::drawVelocity()
{
	if (!vars::misc->plots->enabledVelocity)
		return;

	if (!interfaces::engine->isInGame() || !game::localPlayer->isAlive())
		return;

	static float MAX_SPEEED_MOVE = interfaces::cvar->findVar(XOR("sv_maxspeed"))->getFloat(); // should be accurate
	static bool transparent = false;

	if (vars::misc->plots->velocityCustom)
	{
		if (ImGui::Begin(XOR("Sliders for velocity plot"), &vars::misc->plots->velocityCustom, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			//ImGui::SliderFloat(XOR("Set max velocity"), &MAX_SPEEED_MOVE, 450.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // who will ever get this velocity? adjust if needed
			ImGui::SliderFloat(XOR("Acceptance multiply vel"), &m_acceptanceVelocity, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::ColorPicker(XOR("Color lines plot vel"), &vars::misc->plots->colorVelocity);
			ImGui::Checkbox(XOR("Run transparent"), &transparent);
			ImGui::SameLine();
			ImGui::HelpMarker(XOR("Will add some flags!\nEg: no resize"));

			ImGui::End();
		}
	}

	int flags = transparent
		? ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		: ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin(FORMAT(XOR("Plot Velocity AVG {:.2f}###Plot Velocity"),
		std::reduce(m_VelocityRecords.begin(), m_VelocityRecords.end()) / m_VelocityRecords.size()).c_str(), &vars::misc->plots->enabledVelocity, flags))
	{
		imRenderWindow.addList(); // correct pos, so we start from x = 0 y = 0	
		float acceptance = imRenderWindow.getWidth() / static_cast<float>(RECORDS_SIZE / m_acceptanceVelocity);
		float prevX = 0.0f, prevY = 0.0f;
		float prevVel = 0.0f;
		float scaledavg = MAX_SPEEED_MOVE / imRenderWindow.getHeight(); // might run some better logic...
		float scaledFontSize = imRenderWindow.getWidth() / 25.0f;
		float width = imRenderWindow.getWidth();
		float height = imRenderWindow.getHeight();

		for (size_t i = 0; const auto currentVel : m_VelocityRecords)
		{
			float deltaVel = std::abs(currentVel - prevVel);

			float currentX = i * acceptance; // no need for clamping

			float currentY = imRenderWindow.getHeight() - (currentVel / scaledavg); // is there any better way for this?
			if (currentY < 1.0f)
				currentY = 1.0f; // stay on top to clamp

			// start drawing after there is any change, we could use polylines here
			if (i > 0)
			{
				imRenderWindow.drawLine(currentX, currentY, prevX, prevY, vars::misc->plots->colorVelocity());
				if (deltaVel > 20) // would need proper edge detection
					imRenderWindow.drawText(currentX + 10.0f, currentY - 10.0f, scaledFontSize, ImFonts::franklinGothic30,
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
				const auto alpha = ImGui::ColorConvertU32ToFloat4(Color::U32(color)).z;
				drawing->AddText(font, size, { pos.x + 1.0f, pos.y + 1.0f }, Color::U32(Colors::Black.getColorEditAlpha(alpha)), text.c_str());
			}
			drawing->AddText(font, size, pos, Color::U32(color), text.c_str());
		};

		text(imRenderWindow.getPos().x + width / 2.0f, imRenderWindow.getPos().y + 20.0f + height, 30.0f, ImFonts::franklinGothic30,
			FORMAT(XOR("{}"), std::round(game::localPlayer->m_vecVelocity().toVecPrev().length())), true, Colors::Pink, false);

		imRenderWindow.end();
		ImGui::End();
	}
}

void VelocityGather::run(CUserCmd* cmd)
{
	if (!vars::misc->plots->enabledVelocity)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame() || !game::localPlayer->isAlive())
	{
		g_Plots->m_VelocityRecords.clear();
		return;
	}

	float vel = game::localPlayer->m_vecVelocity().toVecPrev().length();
	if (std::isinf(vel))
	{
		LOG_WARN("record for plot got skipped due to infinity");
		return;
	}

	g_Plots->m_VelocityRecords.emplace_back(game::localPlayer->m_vecVelocity().toVecPrev().length());

	// width
	while (g_Plots->m_VelocityRecords.size() > static_cast<size_t>(g_Plots->RECORDS_SIZE / g_Plots->m_acceptanceVelocity))
		g_Plots->m_VelocityRecords.pop_front();
}
