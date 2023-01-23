#include "hooks.hpp"

#include <cheats/features/events/events.hpp>

hooks::fireIntern::value FASTCALL hooks::fireIntern::hooked(FAST_ARGS, IGameEvent* event, bool server, bool client)
{
	/*for (const auto [name, funcs] : EventCallback::getCallbacks())
	{
		if (const std::string_view ename{ event->getName() }; ename == name)
		{
			for (const auto& func : funcs)
				func(event);
		}
	}*/

	original(thisptr, event, server, client);
}