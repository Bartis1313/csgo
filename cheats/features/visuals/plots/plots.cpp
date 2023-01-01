#include "plots.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <game/game.hpp>
#include <menu/GUI-ImGui/animations.hpp>
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
		realfps = 0.9f * realfps + (1.0f - 0.9f) * memory::interfaces::globalVars->m_absoluteframetime;

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

	static float MAX_FPS = memory::interfaces::engine->isInGame() ? 350.0f : 120.0f;

	if (vars::misc->plots->fpsCustom)
	{
		if (ImGui::Begin(XOR("Sliders for plot"), &vars::misc->plots->fpsCustom, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (memory::interfaces::engine->isInGame()) // not enaled in menu
				ImGui::Animations::SliderFloat(XOR("Set max FPS"), &MAX_FPS, 30.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // hardcoded ranges, we should maybe run some avg fps getter
			ImGui::Animations::SliderFloat(XOR("Acceptance multiply fps"), &acceptanceCustom, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::Animations::ColorPicker(XOR("Color lines plot fps"), &vars::misc->plots->colorFPS);

			ImGui::End();
		}
	}

	if (ImGui::Begin(FORMAT(XOR("Plot FPS AVG {:.2f}###Plot FPS"),
		std::reduce(records.begin(), records.end()) / records.size()).c_str(), &vars::misc->plots->enabledFps, ImGuiWindowFlags_NoCollapse))
	{
		auto& style = ImGui::GetStyle();
		const bool backupLines = style.AntiAliasedLines;
		style.AntiAliasedLines = false;
		const auto windowSize = ImGui::GetContentRegionAvail();
		const auto windowPos = ImGui::GetCursorScreenPos();
		const auto windowList = ImGui::GetWindowDrawList();

		const float acceptance = windowSize.x / static_cast<float>(RECORDS_SIZE / acceptanceCustom);
		const float scaledavg = MAX_FPS / windowSize.y;
		std::vector<ImVec2> points;
		for (size_t i = 0; const auto currentFPS : records)
		{
			const float currentX = i * acceptance;
			const float currentY = std::max(windowSize.y - (currentFPS / scaledavg), 1.0f);
			points.emplace_back(windowPos.x + currentX, windowPos.y + currentY);

			i++;
		}
		drawing::Polyline{ points, Color::U32(vars::misc->plots->colorFPS()), 0, 1.0f }.draw(windowList);
		
		style.AntiAliasedLines = backupLines;
		ImGui::End();
	}
}

void Plots::drawVelocity()
{
	if (!vars::misc->plots->enabledVelocity)
		return;

	if (!memory::interfaces::engine->isInGame() || !game::localPlayer->isAlive())
		return;

	static float MAX_SPEEED_MOVE = memory::interfaces::cvar->findVar(XOR("sv_maxspeed"))->getFloat(); // should be accurate
	static bool transparent = false;

	if (vars::misc->plots->velocityCustom)
	{
		if (ImGui::Begin(XOR("Sliders for velocity plot"), &vars::misc->plots->velocityCustom, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			//ImGui::Animated::SliderFloat(XOR("Set max velocity"), &MAX_SPEEED_MOVE, 450.0f, 1000.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic); // who will ever get this velocity? adjust if needed
			ImGui::Animations::SliderFloat(XOR("Acceptance multiply vel"), &m_acceptanceVelocity, 0.2f, 20.0f, XOR("%.2f"), ImGuiSliderFlags_Logarithmic);
			ImGui::Animations::ColorPicker(XOR("Color lines plot vel"), &vars::misc->plots->colorVelocity);
			ImGui::Animations::Checkbox(XOR("Run transparent"), &transparent);
			ImGui::SameLine();
			ImGui::HelpMarker(XOR("Will add some flags!\nEg: no resize"));

			ImGui::End();
		}
	}

	const int flags = transparent
		? ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		: ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin(FORMAT(XOR("Plot Velocity AVG {:.2f}###Plot Velocity"),
		std::reduce(m_VelocityRecords.begin(), m_VelocityRecords.end()) / m_VelocityRecords.size()).c_str(), &vars::misc->plots->enabledVelocity, flags))
	{
		auto& style = ImGui::GetStyle();
		const bool backupLines = style.AntiAliasedLines;
		style.AntiAliasedLines = false;
		const auto windowSize = ImGui::GetContentRegionAvail();
		const auto windowPos = ImGui::GetCursorScreenPos();
		const auto windowList = ImGui::GetWindowDrawList();

		const float acceptance = windowSize.x / static_cast<float>(RECORDS_SIZE / m_acceptanceVelocity);
		const float scaledavg = MAX_SPEEED_MOVE / windowSize.y;
		std::vector<ImVec2> points;
		for (size_t i = 0; const auto currentVel : m_VelocityRecords)
		{
			const float currentX = i * acceptance;
			const float currentY = std::max(windowSize.y - (currentVel / scaledavg), 1.0f);
			points.emplace_back(windowPos.x + currentX, windowPos.y + currentY);

			i++;
		}
		drawing::Polyline{ points, Color::U32(vars::misc->plots->colorVelocity()), 0, 1.0f }.draw(windowList);
		
		style.AntiAliasedLines = backupLines;
		ImGui::End();
	}
}

void VelocityGather::run(CUserCmd* cmd)
{
	if (!vars::misc->plots->enabledVelocity)
		return;

	if (!game::localPlayer)
		return;

	if (!memory::interfaces::engine->isInGame() || !game::localPlayer->isAlive())
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
