#include "hooks.hpp"

#include "../features/fakelatency/fakelatency.hpp"

int FASTCALL hooks::sendDatagram::hooked(FAST_ARGS, void* datagram)
{
	return FakeLatency::runDatagram(reinterpret_cast<INetChannel*>(thisptr), datagram);
}