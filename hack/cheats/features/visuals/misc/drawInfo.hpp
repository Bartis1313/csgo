#pragma once

#include <cheats/classes/renderableToPresent.hpp>

class IGameEvent;

class MiscInfo : protected RenderablePresentType
{
public:
	constexpr MiscInfo() :
		RenderablePresentType{}
	{}

protected:
	virtual void init() override;
	virtual void draw() override;
private:
	void addHits(IGameEvent* event);
	void resetHits(IGameEvent* event);
	uint32_t m_allHits;
};

GLOBAL_FEATURE(MiscInfo);