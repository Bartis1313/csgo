#include "hooks.hpp"
#include "helpers/helper.hpp"
#include "wndproc.hpp"

#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>

#include "addEntity.hpp"
#include "allocKeyvalues.hpp"
#include "buildTransformations.hpp"
#include "cHudIsHidden.hpp"
#include "createEvent.hpp"
#include "createMove.hpp"
#include "doExtraBonesProcessing.hpp"
#include "doPostScreenEffects.hpp"
#include "drawIndexPrimitive.hpp"
#include "drawModelExecute.hpp"
#include "filesCheck.hpp"
#include "frameStageNotify.hpp"
#include "getColorModulation.hpp"
#include "getUnverifiedFileHashes.hpp"
#include "isDepth.hpp"
#include "isFollowingEntity.hpp"
#include "isHltv.hpp"
#include "isUsingStaticPropDebugModes.hpp"
#include "levelInitPostEnttity.hpp"
#include "levelInitPreEntity.hpp"
#include "levelShutdown.hpp"
#include "lockCursor.hpp"
#include "overrideMouse.hpp"
#include "overrideView.hpp"
#include "paintTraverse.hpp"
#include "particlesSimulation.hpp"
#include "playStepSound.hpp"
#include "present.hpp"
#include "processSpottedEntityUpdate.hpp"
#include "removeEntity.hpp"
#include "reset.hpp"
#include "screen2dEffect.hpp"
#include "sendDatagram.hpp"
#include "svcheats.hpp"
#include "unknownFileSys.hpp"
#include "unknownOverviewMap.hpp"
#include "unknownPlayerHurt.hpp"
#include "unkownRoundEnd.hpp"
#include "viewFade.hpp"
#include "viewRender.hpp"
#include "updatePostEffects.hpp"
#include "drawSetColor.hpp"

#define HOOK(target, _struct) \
	hookHelper::MinHook::tryHook(target, &_struct::hook, hookHelper::ORIGINAL(_struct::original), #_struct);

void hooks::init()
{
	hookHelper::MinHook::initMinhook();

	hooks::Present::original = memory::Address<hooks::Present::call>{ memory::present() }.deRef(memory::Dereference::TWICE)();
	hooks::Reset::original = memory::Address<hooks::Reset::call>{ memory::reset() }.deRef(memory::Dereference::TWICE)();
	**reinterpret_cast<void***>(memory::present()) = Present::hook;
	**reinterpret_cast<void***>(memory::reset()) = Reset::hook;

	//HOOK(vfunc::getVFunc(memory::interfaces::dx9Device(), DRAW_IDX_PRIMITIVE), hooks::DrawIndexedPrimitive);
	HOOK(vfunc::getVFunc(memory::interfaces::keyValuesSys(), ALLOC_KEYVALUE), hooks::AllocKeyValues);

	HOOK(memory::isUsingPropDebug(), hooks::IsUsingStaticPropDebugModes);
	HOOK(memory::getColorModulation(), hooks::GetColorModulation);
	HOOK(memory::extraBonesProcessing(), hooks::DoExtraBonesProcessing);
	HOOK(memory::buildTransformations(), hooks::BuildTransformations);
	HOOK(memory::particleSimulate(), hooks::ParticlesSimulations);
	HOOK(memory::sendDataGram(), hooks::SendDatagram);
	HOOK(memory::unkOverviewMap(), hooks::UnknownOverviewMap);
	HOOK(memory::isDepth(), hooks::IsDepthOfField);
	HOOK(memory::addEnt(), hooks::AddEntity);
	HOOK(memory::removeEnt(), hooks::RemoveEntity);
	HOOK(vfunc::getVFunc(memory::interfaces::client(), FRAMESTAGE), hooks::FrameStageNotify);
	HOOK(vfunc::getVFunc(memory::interfaces::client(), LEVEL_INIT_PREENT), hooks::LevelInitPreEntity);
	HOOK(vfunc::getVFunc(memory::interfaces::client(), LEVEL_INIT_POSTENT), hooks::LevelInitPostEntity);
	HOOK(vfunc::getVFunc(memory::interfaces::client(), LEVEL_SHUTDOWN), hooks::LevelShudown);
	HOOK(vfunc::getVFunc(memory::interfaces::panel(), PAINTTRAVERSE), hooks::PaintTraverse);
	HOOK(vfunc::getVFunc(memory::interfaces::modelRender(), DRAWMODEL), hooks::DrawModelExecute);
	HOOK(vfunc::getVFunc(memory::interfaces::clientMode(), CREATEMOVE), hooks::CreateMove);
	HOOK(vfunc::getVFunc(memory::interfaces::clientMode(), OVERRIDE), hooks::OverrideView);
	HOOK(vfunc::getVFunc(memory::interfaces::clientMode(), OVERRIDE_MOUSE), hooks::OverrideMouse);
	HOOK(vfunc::getVFunc(memory::interfaces::clientMode(), POSTSCREENEFFECT), hooks::DoPostScreenEffects);
	HOOK(vfunc::getVFunc(memory::interfaces::surface(), LOCK_CURSOR), hooks::LockCursor);
	HOOK(vfunc::getVFunc(memory::interfaces::cvar->findVar("sv_cheats"), SV_CHEATS), hooks::SvCheats);
	HOOK(vfunc::getVFunc(memory::interfaces::fileSystem(), UNK_ALLOC_FILESYS), hooks::UnknownFileSysAlloc);
	HOOK(vfunc::getVFunc(memory::interfaces::fileSystem(), LOSE_FILES_CHECK), hooks::FilesCheck);
	HOOK(vfunc::getVFunc(memory::interfaces::fileSystem(), UNVERIFIED_FILE_HASHES), hooks::GetUnverifiedFileHashes);
	HOOK(vfunc::getVFunc(memory::interfaces::viewRender(), RENDER_VIEW), hooks::ViewRender);
	HOOK(vfunc::getVFunc(memory::interfaces::viewRender(), SCREEN_2D_EFFECT), hooks::Screen2DEffect);
	HOOK(vfunc::getVFunc(memory::interfaces::engine(), IS_HLTV), hooks::IsHltv);
	HOOK(memory::isFollowedEntity(), hooks::IsFollowingEntity);
	HOOK(memory::spottedEntityUpdate(), hooks::ProcessSpottedEntityUpdate);
	HOOK(vfunc::getVFunc(memory::interfaces::eventManager(), CREATE_EVENT), hooks::CreateEvent);
	HOOK(memory::preRound(), hooks::UnknownRoundEnd);
	HOOK(memory::playSoundStep(), hooks::PlayStepSound);
	HOOK(memory::chudIsHidden(), hooks::CHudIsHidden);
	HOOK(memory::viewFade(), hooks::ViewFade);
	HOOK(memory::unkRound(), hooks::UnknownPlayerHurt);
	HOOK(memory::updatePostEffects(), hooks::UpdatePostEffects);
	HOOK(vfunc::getVFunc(memory::interfaces::surface(), SET_DRAW_COLOR), hooks::setDrawColor);

#undef HOOK

	hookHelper::MinHook::checkAllHooks();

	console::debug("hooks success");
}

void hooks::shutdown()
{
	**reinterpret_cast<void***>(memory::present()) = hooks::Present::original;
	**reinterpret_cast<void***>(memory::reset()) = hooks::Reset::original;

	hookHelper::MinHook::shutdownAllHooks();
}