#include "unkownRoundEnd.hpp"

#include <SDK/IGameEvent.hpp>

hooks::UnknownRoundEnd::value hooks::UnknownRoundEnd::hook(HACK_FAST_ARGS, void* _event)
{
	const auto ev = reinterpret_cast<IGameEvent*>(_event);
	const std::string_view name = ev->getName();
	
	if (name == "round_end")
	{
		Storage::runs.run();
	}

	original(thisptr, _event);
}