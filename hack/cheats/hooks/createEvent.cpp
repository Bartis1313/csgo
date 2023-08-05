#include "createEvent.hpp"

hooks::CreateEvent::value HACK_FASTCALL hooks::CreateEvent::hook(HACK_FAST_ARGS, const char* name, bool allowed, uint32_t unk)
{
	//allowed = true;

	return original(thisptr, name, allowed, unk);
}