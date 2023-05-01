#pragma once

namespace setup
{
	bool init([[maybe_unused]] void* instance);
	void looper([[maybe_unused]] void* instance);
	void shutdown([[maybe_unused]] void* instance);

	inline bool inited{ false };
};