#include "unkownRoundEnd.hpp"

#include <SDK/IGameEvent.hpp>

hooks::UnknownRoundEnd::value hooks::UnknownRoundEnd::hook(FAST_ARGS, void* _event)
{
	const auto ev = reinterpret_cast<IGameEvent*>(_event);
	const auto name = ev->getName();
	if (name == "round_end")
		Storage::runs.run();

	original(thisptr, _event);
}