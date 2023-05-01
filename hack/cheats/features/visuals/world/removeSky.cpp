#include "removeSky.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/Convar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

#include <cheats/hooks/doPostScreenEffects.hpp>

namespace
{
	struct SkyHandler : hooks::DoPostScreenEffects
	{
		SkyHandler()
		{
			this->registerInit(sky::init);
			this->registerRun(sky::run);
		}
	} skyHandler;
}

namespace sky
{
	IConVar* r_3dsky;
}

void sky::init()
{
	r_3dsky = memory::interfaces::cvar->findVar("r_3dsky");
}

void sky::run()
{
	// do smth about sky controller as well, TODO

	vars::visuals->world->sky->removeSky ? r_3dsky->setValue(false) : r_3dsky->setValue(true);
}
