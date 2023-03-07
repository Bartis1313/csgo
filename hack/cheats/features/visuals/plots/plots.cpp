#include "plots.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <menu/GUI-ImGui/animations.hpp>
#include <config/vars.hpp>
#include <utilities/utilities.hpp>
#include <render/render.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <implot.h>

#include <numeric>

void Plots::init()
{
	m_VelocityRecords.reserve(MAX_SIZE_PLOTS);
	m_FpsRecords.reserve(MAX_SIZE_PLOTS);

	std::iota(m_sharedXS.begin(), m_sharedXS.end(), 0);
}

void Plots::draw()
{
	drawFps();
	drawVelocity();
}

void Plots::drawFps()
{
	if (!vars::misc->plots->enabledFps)
		return;

	auto getfps = []() -> double // this is how they do it
	{
		static double realfps = 0.0;
		realfps = 0.9 * realfps + (1.0 - 0.9) * memory::interfaces::globalVars->m_absoluteframetime;

		return 1.0 / realfps;
	};

	const double fps = getfps();
	// when loading, you have a chance of freezing
	if (std::isinf(fps))
	{
		console::warn("record for plot got skipped due to infinity");
		return;
	}

	m_FpsRecords.emplace_back(fps);

	while (m_FpsRecords.size() > MAX_SIZE_PLOTS)
		m_FpsRecords.erase(m_FpsRecords.begin());

	static float MAX_FPS = memory::interfaces::engine->isInGame() ? 350.0f : 120.0f;

	if (ImGui::Begin("##DummyFps", &vars::misc->plots->enabledFps, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::Animations::SliderFloat("Set max FPS", &MAX_FPS, 30.0f, 1000.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
				ImGui::Animations::SliderInt("Records size", &vars::misc->plots->sizeFps, 10, 1000);
				ImGui::Animations::ColorPicker("Color lines plot fps", &vars::misc->plots->colorFPS);

				ImGui::EndMenu();
			}
	
			ImGui::EndMenuBar();
		}

		if (m_FpsRecords.size() != MAX_SIZE_PLOTS)
		{
			drawing::TextSize{ 30.0f, ImFonts::franklinGothic30, ImGui::GetCurrentWindow()->DC.CursorPos,
				Color::U32(Colors::Yellow), "Wait for buffer to load!", false, false }.draw(ImGui::GetWindowDrawList());

			ImGui::End();
			return;
		}

		std::vector<double> xs{ m_sharedXS.begin(), m_sharedXS.begin() + vars::misc->plots->sizeFps };
		std::vector<double> fps{ m_FpsRecords.end() - vars::misc->plots->sizeFps, m_FpsRecords.end() };

		ImPlot::SetNextAxesLimits(0, vars::misc->plots->sizeFps, 0, MAX_FPS, ImPlotCond_::ImPlotCond_Always);
		ImPlot::PushStyleColor(ImPlotCol_Fill, Color::getImguiColor(vars::misc->plots->colorFPS().getColorEditAlpha(0.25f)));
		ImPlot::PushStyleColor(ImPlotCol_Line, Color::getImguiColor(vars::misc->plots->colorFPS()));
		ImPlot::PushStyleColor(ImPlotCol_FrameBg, { 0,0,0,0 });
		ImPlot::PushStyleColor(ImPlotCol_PlotBg, { 0,0,0,0 });

		if (ImPlot::BeginPlot("Fps", ImVec2{ -1, -1 }))
		{

			ImPlot::SetupAxes("Records", "Fps");		
			ImPlot::PlotShaded("##plotfps", xs.data(), fps.data(), vars::misc->plots->sizeFps);
			ImPlot::PlotLine("##plotfps", xs.data(), fps.data(), vars::misc->plots->sizeFps);

			ImPlot::EndPlot();
		}

		ImPlot::PopStyleColor(4);

		ImGui::End();
	}
}

void Plots::drawVelocity()
{
	if (!vars::misc->plots->enabledVelocity)
		return;

	if (!memory::interfaces::engine->isInGame() || !game::localPlayer->isAlive())
		return;

	static float MAX_SPEEED_MOVE = memory::interfaces::cvar->findVar("sv_maxspeed")->getFloat(); // should be accurate

	if (m_VelocityRecords.empty())
		return;

	std::vector<double> xs(m_VelocityRecords.size());
	for (size_t i = 0; auto& el : xs)
	{
		el = i;
		i++;
	}

	const int flags = vars::misc->plots->transparencyVelocity
		? ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize
		: ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin("##DummyVel", &vars::misc->plots->enabledVelocity, flags))
	{
		if (!vars::misc->plots->transparencyVelocity)
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::Animations::SliderInt("Records size", &vars::misc->plots->sizeVelocity, 10, MAX_SIZE_PLOTS - 1);
					ImGui::Animations::ColorPicker("Color lines plot vel", &vars::misc->plots->colorVelocity);

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}

		if (m_VelocityRecords.size() != MAX_SIZE_PLOTS)
		{
			drawing::TextSize{ 30.0f, ImFonts::franklinGothic30, ImGui::GetCurrentWindow()->DC.CursorPos,
				Color::U32(Colors::Yellow), "Wait for buffer to load!", false, false }.draw(ImGui::GetWindowDrawList());

			ImGui::End();
			return;
		}

		std::vector<double> xs{ m_sharedXS.begin(), m_sharedXS.begin() + vars::misc->plots->sizeVelocity };
		std::vector<double> vel{ m_VelocityRecords.end() - vars::misc->plots->sizeVelocity, m_VelocityRecords.end() };

		ImPlot::SetNextAxesLimits(0, vars::misc->plots->sizeVelocity, 0, MAX_SPEEED_MOVE + 20, ImPlotCond_::ImPlotCond_Always);
		ImPlot::PushStyleColor(ImPlotCol_Fill, Color::getImguiColor(vars::misc->plots->colorVelocity().getColorEditAlpha(0.25f)));
		ImPlot::PushStyleColor(ImPlotCol_Line, Color::getImguiColor(vars::misc->plots->colorVelocity()));
		ImPlot::PushStyleColor(ImPlotCol_FrameBg, { 0,0,0,0 });
		ImPlot::PushStyleColor(ImPlotCol_PlotBg, { 0,0,0,0 });

		const int flags = vars::misc->plots->transparencyVelocity ? ImPlotFlags_NoTitle : ImPlotFlags_None;

		if (ImPlot::BeginPlot("Velocity", ImVec2{ -1, -1 }, flags))
		{
			ImPlot::SetupAxes("Records", "Velocity", 
				vars::misc->plots->transparencyVelocity ? ImPlotAxisFlags_NoDecorations : ImPlotAxisFlags_None,
				vars::misc->plots->transparencyVelocity ? ImPlotAxisFlags_NoDecorations : ImPlotAxisFlags_None);

			if(!vars::misc->plots->transparencyVelocity)
				ImPlot::PlotShaded("##plotvel", xs.data(), vel.data(), vars::misc->plots->sizeVelocity);
			ImPlot::PlotLine("##plotvel", xs.data(), vel.data(), vars::misc->plots->sizeVelocity);

			ImPlot::EndPlot();
		}

		ImPlot::PopStyleColor(4);

		ImGui::End();
	}
}

void VelocityGather::run(CUserCmd* cmd)
{
	if (!vars::misc->plots->enabledVelocity)
		return;

	if (!game::localPlayer)
		return;

	if (!memory::interfaces::engine->isInGame())
	{
		g_Plots->m_VelocityRecords.clear();
		return;
	}

	float vel = game::localPlayer->m_vecVelocity().toVecPrev().length();
	if (std::isinf(vel))
	{
		console::warn("record for plot got skipped due to infinity");
		return;
	}

	g_Plots->m_VelocityRecords.emplace_back(game::localPlayer->m_vecVelocity().toVecPrev().length());

	// width
	while (g_Plots->m_VelocityRecords.size() > g_Plots->MAX_SIZE_PLOTS)
		g_Plots->m_VelocityRecords.erase(g_Plots->m_VelocityRecords.begin());
}
