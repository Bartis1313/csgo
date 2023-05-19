#pragma once

class CUserCmd;
class INetChannel;

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/227226-synced-fake-ping.html
namespace fakeLatency
{
	void init();
	int runDatagram(INetChannel* netChannel, void* datagram);
	void run([[maybe_unused]] CUserCmd* cmd);
};