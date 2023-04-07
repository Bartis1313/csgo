#pragma once

#include <vector>
#include <memory>

// a base class for creating hacks, see folder "classes" and features and example how to use it.
// we should not care what is first as init or last, those things to init should not depend at all.
class BaseHack
{
public:
	constexpr BaseHack() { m_hacks.push_back(this); }

	static void initAll();
	static void resetAll();
	static void loadAll();
	static void shutdownAll();
	static void restartRoundAll();
protected:
	virtual void init() = 0;
	virtual void reset() = 0;
	virtual void load() = 0;
	virtual void shutdown() = 0;
	virtual void roundRestart() = 0;
private:
	inline static std::vector<std::add_pointer_t<BaseHack>> m_hacks;
};

#define GLOBAL_FEATURE(className) [[maybe_unused]] inline auto g_##className{ std::make_unique<className>() };