#pragma once

class DiscordPresence final
{
public:
	void run();
	void init();
	void shutdown();
};

inline DiscordPresence dc;