#include "createEvent.hpp"

hooks::CreateEvent::value FASTCALL hooks::CreateEvent::hook(FAST_ARGS, const char* name, bool allowed, uint32_t unk)
{
	//allowed = true;

	return original(thisptr, name, allowed, unk);
}