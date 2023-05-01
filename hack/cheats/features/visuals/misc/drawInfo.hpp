#pragma once

class IGameEvent;

namespace miscInfo
{
	void addHits(IGameEvent* event);
	void init();
	void draw();
	void reset();

	inline uint32_t m_allHits;
}