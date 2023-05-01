#include "sendDatagram.hpp"

#include <cheats/features/fakelatency/fakelatency.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>

hooks::SendDatagram::value hooks::SendDatagram::hook(FAST_ARGS, void* datagram)
{
	if (datagram || !vars::misc->fakeLatency->enabled || !game::isAvailable())
		return original(thisptr, datagram);

	return fakeLatency::runDatagram(static_cast<INetChannel*>(thisptr), datagram);
}