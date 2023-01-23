#pragma once

#include <cheats/classes/onlyInit.hpp>

#include <utility>

class Player_t;
class IConVar;

class EnemyWarning : protected OnlyInitType
{
public:
	constexpr EnemyWarning() :
		OnlyInitType{}
	{}

	std::pair<bool, bool> check(Player_t* ent);
	void draw(const std::pair<bool, bool>& checks);
protected:
	virtual void init() override;
private:
	IConVar* m_scale;
};

GLOBAL_FEATURE(EnemyWarning);
