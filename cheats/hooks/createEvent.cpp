#include "hooks.hpp"

IGameEvent* FASTCALL hooks::createEvent::hooked(FAST_ARGS, const char* name, bool allowed, uint32_t unk)
{
	//allowed = true;

	return original(thisptr, name, allowed, unk);
}