#pragma once

#include <utilities/runnable.hpp>

#include <setup/setup.hpp>

class InitAble
{
	friend bool setup::init(void* instance);
protected:
	static void registerInit(const std::function<void()>& func) { Storage::runs.push_back(func); }
private:
	struct Storage
	{
		inline static vectorRunnable<std::function<void()>> runs;
	};
};