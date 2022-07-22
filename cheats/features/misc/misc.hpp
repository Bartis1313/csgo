#pragma once

#include <deque>
#include <vector>

#include "../../../SDK/math/Vector.hpp"

class IGameEvent;
class CFlashlightEffect;

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
	void drawBulletTracer(const Vector& start, const Vector& end);
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/255782-cs-flash-light.html + kept less asm and updated everything
	void flashLight(int frame);
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
		Vector m_pos;

		bool isAvailable() const { return m_health >= 0; }
	};
	std::vector<Hitmark_t> m_hitmarkers;
	CFlashlightEffect* m_flashLight = nullptr;
};

inline Misc misc;