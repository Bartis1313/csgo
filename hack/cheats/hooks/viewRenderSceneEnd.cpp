#include "hooks.hpp"

hooks::sceneEnd::value FASTCALL hooks::sceneEnd::hooked(FAST_ARGS)
{
	original(thisptr);
}