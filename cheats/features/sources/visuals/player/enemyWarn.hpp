#pragma once

#include "../../../classes/onlyInit.hpp"

class Player_t;
class IConVar;

class EnemyWarning : public OnlyInitType
{
public:
	EnemyWarning()
	{
		m_hacks.push_back(this);
	}

	virtual void init();
	void draw(Player_t* ent);
private:
	IConVar* m_scale;
};

[[maybe_unused]] inline auto g_EnemyWarning = EnemyWarning{};
