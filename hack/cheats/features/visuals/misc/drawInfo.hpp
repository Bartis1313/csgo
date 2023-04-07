#pragma once

#include <cheats/classes/renderableToPresent.hpp>

class IGameEvent;

class MiscInfo : protected RenderablePresentType
{
public:
	constexpr MiscInfo() :
		RenderablePresentType{}
	{}

	void addHits(IGameEvent* event);
protected:
	virtual void init() override;
	virtual void draw() override;
	virtual void reset() override;
private:
	uint32_t m_allHits;
};

GLOBAL_FEATURE(MiscInfo);