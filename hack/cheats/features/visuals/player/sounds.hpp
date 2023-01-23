#pragma once

#include <cheats/classes/onlyInit.hpp>
#include <SDK/math/Vector.hpp>

#include <array>
#include <deque>

class IGameEvent;
class Entity_t;
class Player_t;

class SoundDraw
{
public:
	static void draw();
	static void findBest(Player_t* ent);
	static void pushSteps(Player_t* ent);
private:
	struct StepData_t
	{
		StepData_t() = default;
		constexpr StepData_t(Player_t* player, const Vec3& pos, float expire)
			: m_player{ player }, m_pos{ pos }, m_expire{ expire }
		{}

		Player_t* m_player = nullptr;
		Vec3 m_pos;
		float m_expire;
		float m_maxPixels;
		float m_timeToPrint;
		float m_fontSize;
	};

	inline static std::array<std::deque<StepData_t>, 65> m_steps;
	inline static StepData_t m_bestStep;
};
