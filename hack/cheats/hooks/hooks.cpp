#include "hooks.hpp"
#include "helpers/helper.hpp"
#include "wndproc.hpp"

#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>

void hooks::init()
{
#define HOOK(target, name) \
if constexpr (name::index != hooks::indexNone) { \
hookHelper::tryHook(vfunc::getVFunc(target, name::index), &name::hooked, \
	hookHelper::ORIGINAL(name::original), #name); \
} \
else { \
	hookHelper::tryHook(target, &name::hooked, \
	hookHelper::ORIGINAL(name::original), #name); \
}
	hookHelper::initMinhook();

	HOOK(memory::interfaces::dx9Device(), hooks::reset);
	HOOK(memory::interfaces::dx9Device(), hooks::present);
	HOOK(memory::interfaces::dx9Device(), hooks::drawIndexedPrimitive);

	HOOK(memory::interfaces::keyValuesSys(), hooks::allocKeyValues);
	HOOK(memory::isUsingPropDebug(), hooks::isUsingStaticPropDebugModes);
	HOOK(memory::getColorModulation(), hooks::getColorModulation);
	HOOK(memory::extraBonesProcessing(), hooks::doExtraBonesProcessing);
	HOOK(memory::buildTransformations(), hooks::buildTransformations);
	HOOK(memory::particleSimulate(), hooks::particlesSimulations);
	HOOK(memory::sendDataGram(), hooks::sendDatagram);
	HOOK(memory::unkOverviewMap(), hooks::unknownOverViewFun);
	HOOK(memory::isDepth(), hooks::isDepthOfField);
	HOOK(memory::addEnt(), hooks::addEnt);
	HOOK(memory::removeEnt(), hooks::removeEnt);	
	HOOK(memory::interfaces::client(), hooks::frameStageNotify);
	HOOK(memory::interfaces::client(), hooks::levelInitPreEntity);
	HOOK(memory::interfaces::client(), hooks::levelInitPostEntity);
	HOOK(memory::interfaces::client(), hooks::levelShutdown);	
	HOOK(memory::interfaces::panel(), hooks::paintTraverse);
	HOOK(memory::interfaces::modelRender(), hooks::drawModel);
	HOOK(memory::interfaces::clientMode(), hooks::createMove);
	HOOK(memory::interfaces::clientMode(), hooks::overrideView);
	HOOK(memory::interfaces::clientMode(), hooks::overrideMouse);
	HOOK(memory::interfaces::clientMode(), hooks::doPostScreenEffects);
	HOOK(memory::interfaces::surface(), hooks::lockCursor);
	HOOK(memory::interfaces::cvar->findVar("sv_cheats"), hooks::sv_cheats);
	HOOK(memory::interfaces::fileSystem(), hooks::unknownFileSystemAlloc);
	HOOK(memory::interfaces::fileSystem(), hooks::filesCheck);
	HOOK(memory::interfaces::fileSystem(), hooks::getUnverifiedFileHashes);
	HOOK(memory::interfaces::viewRender(), hooks::viewRender);
	HOOK(memory::interfaces::viewRender(), hooks::screen2DEffect);
	HOOK(memory::interfaces::engine(), hooks::isHltv);
	HOOK(memory::isFollowedEntity(), hooks::isFollowingEntity);
	HOOK(memory::spottedEntityUpdate(), hooks::processSpottedEntityUpdate);
	HOOK(memory::interfaces::sound(), hooks::emitSound);
	HOOK(memory::interfaces::eventManager(), hooks::createEvent);
	HOOK(memory::fireInternfn(), hooks::fireIntern);
	HOOK(memory::interfaces::prediction(), hooks::runCommand);
	HOOK(memory::preRound(), hooks::preRestartRound);
	HOOK(memory::playSoundStep(), hooks::playStepSound);
	HOOK(memory::interfaces::renderView(), hooks::sceneEnd);
	HOOK(memory::chudIsHidden(), hooks::chudIsHidden);
	HOOK(memory::viewFade(), hooks::viewDrawFade);
	HOOK(memory::unkRound(), hooks::unkround);

#undef HOOK

	hookHelper::checkAllHooks();

	console::debug("hooks success");
}

void hooks::shutdown()
{
	hookHelper::shutdownAllHooks();
}