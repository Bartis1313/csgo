#pragma once
#include <vector>
#include "../../../SDK/interfaces/interfaces.hpp"

namespace misc
{
	void thirdperson();
	void drawCrosshair();
	void drawLocalInfo();
	// PLOTS: if anyone can fix it to work with polylines I am impressed
	// the result I got: https://cdn.discordapp.com/attachments/826945218355658753/911933573211250708/unknown.png
	void drawFpsPlot();
	void drawVelocityPlot();
	void getVelocityData();
	void drawHitmarker();
	void playHitmarker(IGameEvent* event);
	inline int hitAlpha = 0;
}