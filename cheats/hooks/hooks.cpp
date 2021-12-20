#include "../../utilities/utilities.hpp"
#include "hooks.hpp"
#include "../menu/menu.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../features/misc/events.hpp"

using namespace utilities;

inline void* TARGET(void* arg, unsigned int index)
{
	return reinterpret_cast<void*>(vfunc::getVFunc(arg, index));
}

template<typename T>
inline void** ORIGINAL(T& arg)
{
	return reinterpret_cast<void**>(&arg);
}

bool hooks::init()
{
	const auto paintTraverseTarget = TARGET(interfaces::panel, paintTraverse::index);
	const auto creteMoveTarget = TARGET(interfaces::clientMode, createMove::index);
	const auto drawModelTarget = TARGET(interfaces::modelRender, drawModel::index);
	const auto overrideViewTarget = TARGET(interfaces::clientMode, overrideView::index);
	const auto doPostScreenEffectsTarget = TARGET(interfaces::clientMode, doPostScreenEffects::index);
	const auto frameStageNotifyTarget = TARGET(interfaces::client, frameStageNotify::index);

	if (MH_Initialize() != MH_OK)
		throw std::runtime_error(XOR("MH_Initialize hook error"));

	if (const auto hk = MH_CreateHook(creteMoveTarget, &createMove::hooked, ORIGINAL(createMove::original)))
	{
		//LOG("")
		if(hk != MH_OK)
			throw std::runtime_error(XOR("createMove hook error"));
	}

	if (const auto hk = MH_CreateHook(paintTraverseTarget, &paintTraverse::hooked, ORIGINAL(paintTraverse::original)))
	{
		if (hk != MH_OK)
			throw std::runtime_error(XOR("paintTraverse hook error"));
	}

	if (const auto hk = MH_CreateHook(drawModelTarget, &drawModel::hooked, ORIGINAL(drawModel::original)))
	{
		if (hk != MH_OK)
			throw std::runtime_error(XOR("drawModel hook error"));
	}

	if (const auto hk = MH_CreateHook(overrideViewTarget, &overrideView::hooked, ORIGINAL(overrideView::original)))
	{
		if (hk != MH_OK)
			throw std::runtime_error(XOR("overrideView hook error"));
	}

	if (const auto hk = MH_CreateHook(doPostScreenEffectsTarget, &doPostScreenEffects::hooked, ORIGINAL(doPostScreenEffects::original)))
	{
		if(hk != MH_OK)
			throw std::runtime_error(XOR("doPostScreenEffects hook error"));
	}

	if (const auto hk = MH_CreateHook(frameStageNotifyTarget, &frameStageNotify::hooked, ORIGINAL(frameStageNotify::original)))
	{
		if(hk != MH_OK)
			throw std::runtime_error(XOR("frameStageNotify hook error"));
	}

	Events::g().init();

	if (const auto hk = MH_EnableHook(MH_ALL_HOOKS); hk != MH_OK)
		throw std::runtime_error(XOR("MH_EnableHook hook error"));

	LOG(LOG_INFO, XOR("hooks initialized!"));

	return true;
}

void hooks::shutdown()
{
	MH_Uninitialize();
	Events::g().shutdown();
	MH_DisableHook(MH_ALL_HOOKS);
}