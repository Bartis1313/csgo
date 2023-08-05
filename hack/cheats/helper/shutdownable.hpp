#pragma once

#include <utilities/runnable.hpp>

#include <cheats/game/game.hpp>

class ShutdownAble
{
	friend void game::shutdown();
protected:
	static void registerShutdown(const std::function<void()>& func) { Storage::shutdowns.push_back(func); }
private:
	struct Storage
	{
		inline static vectorRunnable<std::function<void()>> shutdowns;
	};
};