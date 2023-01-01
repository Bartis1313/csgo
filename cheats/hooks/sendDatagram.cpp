#include "hooks.hpp"

#include "../features/fakelatency/fakelatency.hpp"

hooks::sendDatagram::value FASTCALL hooks::sendDatagram::hooked(FAST_ARGS, void* datagram)
{
	return FakeLatency::runDatagram(reinterpret_cast<INetChannel*>(thisptr), datagram);
}