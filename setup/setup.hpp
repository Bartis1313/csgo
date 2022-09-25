#pragma once

class DiscordPresence;

struct Setup
{
public:
	static bool init([[maybe_unused]] void* instance);
	static void looper([[maybe_unused]] void* instance);
	static void shutdown([[maybe_unused]] void* instance);
private:
	inline static bool m_inited = false;

	// specific for looper
	static DiscordPresence m_dc;
};