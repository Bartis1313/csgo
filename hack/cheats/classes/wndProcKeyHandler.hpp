#pragma once

#include <vector>

// special class, doesn't touch BaseHack at all
// only to update key
class WndProcKeyHandler
{
public:
	constexpr WndProcKeyHandler() { m_allHandlers.push_back(this); }
	static void updateAllKeys();
protected:
	virtual void updateKeys() = 0;
private:
	inline static std::vector<WndProcKeyHandler*> m_allHandlers;
};