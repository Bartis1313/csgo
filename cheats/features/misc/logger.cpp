#include "logger.hpp"

#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../config/vars.hpp"

void Logger::add(const Log_t& log)
{
	if (!config.get<bool>(vars.bLogEnabled))
		return;

	m_logs.push_back(log);
}

void Logger::draw()
{
	if (!config.get<bool>(vars.bLogEnabled))
		return;

	static float y = ImFonts::tahoma14->FontSize;

	for (size_t i = 0; auto & el : m_logs)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_logs.erase(m_logs.begin() + i);
			continue;
		}

		float scale = diff / config.get<float>(vars.fLogMaxTime);

		if(scale > 0.1f) // normal pos
			el.m_y = (i * y);
		else
		{
			el.m_y -= 0.01f * (1.0f / scale); // hardcoded smoothing, text goes up
			el.m_color = el.m_color.getColorEditAlpha(scale * 3.0f); // some alpha up
		}
		
		imRender.text(10.0f, el.m_y, ImFonts::tahoma14, el.m_text, false, el.m_color, false);

		i++;
	}
}