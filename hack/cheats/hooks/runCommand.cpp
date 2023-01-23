#include "hooks.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/IClientState.hpp>
#include <SDK/structs/Entity.hpp>

hooks::runCommand::value FASTCALL hooks::runCommand::hooked(FAST_ARGS, Player_t* player, CUserCmd* cmd, IMoveHelper* moveHelper)
{
	return original(thisptr, player, cmd, moveHelper);
}