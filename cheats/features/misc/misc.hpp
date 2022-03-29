#pragma once

#include <deque>

class IGameEvent;

class Misc final
{
public:
	void thirdperson();
	void drawCrosshair();
	void drawLocalInfo();
	void drawFpsPlot();
	void drawVelocityPlot();
	void getVelocityData();
	void drawHitmarker();
	void playHitmarker(IGameEvent* event);
	void drawNoScope();
	void drawHat();
private:
	int hitAlpha = 0;
	std::deque<float> velRecords;
	constexpr static size_t RECORDS_SIZE_VEL = 300;
	float acceptanceVelocityCustom = 1.0f;
};

inline Misc misc;