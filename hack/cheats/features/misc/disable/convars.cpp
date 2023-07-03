#include "convars.hpp"

#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <config/vars.hpp>
#include <cheats/helper/shutdownable.hpp>

namespace
{
	struct CvarHandler : ShutdownAble
	{
		CvarHandler()
		{
			this->registerShutdown(convars::shutdown);
		}
	} cvarHandler;
}

namespace convars
{
	std::vector<ConCommandBase*> oldVars;

	void runOnce();
}

void convars::runOnce()
{
	const auto itr = memory::interfaces::cvar->factoryInternalIterator();
	for (itr->setFirst(); itr->isValid(); itr->next())
	{
		auto cmd = itr->get();
		if (cmd->isFlagSet(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN))
		{
			oldVars.push_back(cmd);
			cmd->removeFlags(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN);
			cmd->oldFlags &= ~(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN);
		}
	}
}

void convars::run()
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, runOnce);
}

void convars::shutdown()
{
	for (auto var : oldVars)
	{
		var->addFlags(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN);
		var->oldFlags |= (FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN);
	}
}