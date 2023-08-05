#pragma once

#include <utilities/runnable.hpp>

#include <cheats/game/game.hpp>

class InitAble
{
	friend void game::init();
protected:
	static void registerInit(const std::function<void()>& func) { Storage::runs.push_back(func); }
private:
	struct Storage
	{
		inline static vectorRunnable<std::function<void()>> runs;
	};
};