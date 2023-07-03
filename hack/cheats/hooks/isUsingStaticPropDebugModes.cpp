#include "isUsingStaticPropDebugModes.hpp"

#include <config/vars.hpp>

hooks::IsUsingStaticPropDebugModes::value hooks::IsUsingStaticPropDebugModes::hook()
{
	return vars::visuals->world->modulate->enabledProp;
}