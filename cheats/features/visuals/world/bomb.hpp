#pragma once

#include <classes/renderableToPresent.hpp>
#include <classes/frameStage.hpp>

class Bomb_t;
class IConVar;
class IGameEvent;
class BombOverlayEntGrabber;

class BombOverlay : public RenderablePresentType
{
public:
	constexpr BombOverlay() :
		RenderablePresentType{}
	{}

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

	friend BombOverlayEntGrabber;
};

[[maybe_unused]] inline auto g_BombOverlay = BombOverlay{};

class BombOverlayEntGrabber : public FrameStageType
{
public:
	constexpr BombOverlayEntGrabber()
		: FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
};

[[maybe_unused]] inline auto g_BombOverlayEntGrabber = BombOverlayEntGrabber{};
