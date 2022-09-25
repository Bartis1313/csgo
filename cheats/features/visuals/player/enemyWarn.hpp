#pragma once

#include <classes/onlyInit.hpp>

#include <utility>

class Player_t;
class IConVar;

class EnemyWarning : public OnlyInitType
{
public:
	constexpr EnemyWarning() :
		OnlyInitType{}
	{}

	virtual void init();
 	std::pair<bool, bool> check(Player_t* ent);
	void draw(const std::pair<bool, bool>& checks);
private:
	IConVar* m_scale;
};

[[maybe_unused]] inline auto g_EnemyWarning = EnemyWarning{};
