#pragma once

#include <deque>
#include <vector>

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
	std::deque<float> velRecords;
	constexpr static size_t RECORDS_SIZE_VEL = 300;
	float acceptanceVelocityCustom = 1.0f;
	struct Hitmark_t
	{
		float m_expire;
		int m_dmg;
		int m_health;
		bool m_head;

		bool isAvailable() const { return m_health >= 0; }
	};
	std::vector<Hitmark_t> m_hitmarkers;

};

inline Misc misc;