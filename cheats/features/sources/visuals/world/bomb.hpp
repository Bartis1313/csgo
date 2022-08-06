#pragma once

#include "../../../classes/renderableToPresent.hpp"

class Bomb_t;
class IConVar;
class IGameEvent;

class BombOverlay : public RenderablePresentType
{
public:
	BombOverlay()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void draw();
	void setBombEnt(Bomb_t* ent) { m_bombEnt = ent; }
private:
	void handleWhoPlanted(IGameEvent* event);
	void handleBombExplode(IGameEvent* event);
	void handleResetBomb(IGameEvent* event);

	Bomb_t* m_bombEnt;
	IConVar* m_timer;
	std::string m_whoPlanted;
};

[[maybe_unused]] inline auto g_BombOverlay = BombOverlay{};
