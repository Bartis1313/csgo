#pragma once

#include <utilities/runnable.hpp>

#include <setup/setup.hpp>

class ShutdownAble
{
	friend void setup::shutdown(void* instance);
protected:
	static void registerShutdown(const std::function<void()>& func) { Storage::shutdowns.push_back(func); }
private:
	struct Storage
	{
		inline static vectorRunnable<std::function<void()>> shutdowns;
	};
};