#pragma once

#include <classes/onlyInit.hpp>
#include <SDK/math/Vector.hpp>

#include <array>
#include <deque>

class IGameEvent;
class Entity_t;
class Player_t;

class SoundDraw : public OnlyInitType
{
public:
	SoundDraw() :
		OnlyInitType{}
	{}

	virtual void init();
	void draw();
	void findBest(Entity_t* ent);
private:
	struct StepData_t
	{
		StepData_t() = default;
		StepData_t(Player_t* player, const Vector& pos, float expire)
			: m_player{ player }, m_pos{ pos }, m_expire{ expire }
		{}
		Player_t* m_player;
		Vector m_pos;
		float m_expire;
		float m_maxPixels;
		float m_timeToPrint;
		float m_fontSize;
	};
	void pushSteps(IGameEvent* event);

	std::array<std::deque<StepData_t>, 65> m_steps;
	StepData_t m_bestStep;
};

[[maybe_unused]] inline auto g_SoundDraw = SoundDraw{};
