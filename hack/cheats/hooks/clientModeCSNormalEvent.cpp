#include "clientModeCSNormalEvent.hpp"

hooks::ClientModeCSNormalEvent::value hooks::ClientModeCSNormalEvent::hook(FAST_ARGS, IGameEvent* _event)
{
	Storage::runs.run(_event);

	original(thisptr, _event);
}