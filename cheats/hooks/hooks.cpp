#include "hooks.hpp"
#include "../features/misc/events.hpp"
#include "../../dependencies/minhook/Minhook.h"
#include "../../SDK/structs/IDXandPaterrns.hpp"
#include <format>

inline void* TARGET(void* arg, unsigned int index)
{
	return reinterpret_cast<void*>(vfunc::getVFunc(arg, index));
}

template<typename T>
inline void** ORIGINAL(T& arg)
{
	return reinterpret_cast<void**>(&arg);
}

void hooks::init()
{
	const auto paintTraverseTarget = TARGET(interfaces::panel, paintTraverse::index);
	const auto creteMoveTarget = TARGET(interfaces::clientMode, createMove::index);
	const auto drawModelTarget = TARGET(interfaces::modelRender, drawModel::index);
	const auto overrideViewTarget = TARGET(interfaces::clientMode, overrideView::index);
	const auto doPostScreenEffectsTarget = TARGET(interfaces::clientMode, doPostScreenEffects::index);
	const auto frameStageNotifyTarget = TARGET(interfaces::client, frameStageNotify::index);

	const auto addrClient = utilities::patternScan(CLIENT_DLL, NEW_CHECK) + 0x1;
	const auto offsetClient = *reinterpret_cast<uintptr_t*>(addrClient);
	const auto clientValidAddrTarget = reinterpret_cast<void*>(addrClient + 0x4 + offsetClient);

	const auto addrEngine = utilities::patternScan(ENGINE_DLL, NEW_CHECK) + 0x1;
	const auto offsetEngine = *reinterpret_cast<uintptr_t*>(addrEngine);
	const auto engineValidAddrTarget = reinterpret_cast<void*>(addrEngine + 0x4 + offsetEngine);

	const auto addrStudio = utilities::patternScan(STUDIORENDER_DLL, NEW_CHECK) + 0x1;
	const auto offsetStudio = *reinterpret_cast<uintptr_t*>(addrStudio);
	const auto studioValidAddrTarget = reinterpret_cast<void*>(addrStudio + 0x4 + offsetStudio);

	const auto addrMaterial = utilities::patternScan(MATERIAL_DLL, NEW_CHECK) + 0x1;
	const auto offsetMaterial = *reinterpret_cast<uintptr_t*>(addrMaterial);
	const auto matertialValidAddrTarget = reinterpret_cast<void*>(addrMaterial + 0x4 + offsetMaterial);

	if (MH_Initialize() != MH_OK)
		throw std::runtime_error(XOR("MH_Initialize hook error"));

	const auto hk1 = MH_CreateHook(creteMoveTarget, &createMove::hooked, ORIGINAL(createMove::original));
	if (hk1 != MH_OK)
		throw std::runtime_error(XOR("createMove hook error"));
	LOG(LOG_INFO, std::format(XOR("createMove -> {}"), MH_StatusToString(hk1)));

	const auto hk2 = MH_CreateHook(paintTraverseTarget, &paintTraverse::hooked, ORIGINAL(paintTraverse::original));
	if (hk2 != MH_OK)
		throw std::runtime_error(XOR("paintTraverse hook error"));
	LOG(LOG_INFO, std::format(XOR("paintTraverse -> {}"), MH_StatusToString(hk2)));

	const auto hk3 = MH_CreateHook(drawModelTarget, &drawModel::hooked, ORIGINAL(drawModel::original));
	if (hk3 != MH_OK)
		throw std::runtime_error(XOR("drawModel hook error"));
	LOG(LOG_INFO, std::format(XOR("drawModel -> {}"), MH_StatusToString(hk3)));

	const auto hk4 = MH_CreateHook(overrideViewTarget, &overrideView::hooked, ORIGINAL(overrideView::original));
	if (hk4 != MH_OK)
		throw std::runtime_error(XOR("overrideView hook error"));
	LOG(LOG_INFO, std::format(XOR("overrideView -> {}"), MH_StatusToString(hk4)));

	const auto hk5 = MH_CreateHook(doPostScreenEffectsTarget, &doPostScreenEffects::hooked, ORIGINAL(doPostScreenEffects::original));
	if (hk5 != MH_OK)
		throw std::runtime_error(XOR("doPostScreenEffects hook error"));
	LOG(LOG_INFO, std::format(XOR("doPostScreenEffects -> {}"), MH_StatusToString(hk5)));

	const auto hk6 = MH_CreateHook(frameStageNotifyTarget, &frameStageNotify::hooked, ORIGINAL(frameStageNotify::original));
	if (hk6 != MH_OK)
		throw std::runtime_error(XOR("frameStageNotify hook error"));
	LOG(LOG_INFO, std::format(XOR("frameStageNotify -> {}"), MH_StatusToString(hk6)));

	const auto hk7 = MH_CreateHook(clientValidAddrTarget, &clientValidAddr::hooked, ORIGINAL(clientValidAddr::original));
	if (hk7 != MH_OK)
		throw std::runtime_error(XOR("clientValidAddr hook error"));
	LOG(LOG_INFO, std::format(XOR("clientValidAddr -> {}"), MH_StatusToString(hk7)));

	const auto hk8 = MH_CreateHook(engineValidAddrTarget, &engineValidAddr::hooked, ORIGINAL(engineValidAddr::original));
	if (hk8 != MH_OK)
		throw std::runtime_error(XOR("engineValidAddr hook error"));
	LOG(LOG_INFO, std::format(XOR("engineValidAddr -> {}"), MH_StatusToString(hk7)));

	const auto hk9 = MH_CreateHook(studioValidAddrTarget, &studioRenderValidAddr::hooked, ORIGINAL(studioRenderValidAddr::original));
	if (hk7 != MH_OK)
		throw std::runtime_error(XOR("studioValidAddr hook error"));
	LOG(LOG_INFO, std::format(XOR("studioValidAddr -> {}"), MH_StatusToString(hk7)));

	const auto hk10 = MH_CreateHook(matertialValidAddrTarget, &materialSystemValidAddr::hooked, ORIGINAL(materialSystemValidAddr::original));
	if (hk7 != MH_OK)
		throw std::runtime_error(XOR("matertialValidAddr hook error"));
	LOG(LOG_INFO, std::format(XOR("matertialValidAddr -> {}"), MH_StatusToString(hk7)));

	Events::g().init();

	const auto status = MH_EnableHook(MH_ALL_HOOKS);
	if (status != MH_OK)
		throw std::runtime_error(XOR("MH_ALL_HOOKS error"));
	LOG(LOG_INFO, std::format(XOR("MH_ALL_HOOKS -> {}"), MH_StatusToString(status)));

	LOG(LOG_INFO, XOR("hooks initialized!"));
}

void hooks::shutdown()
{
	MH_Uninitialize();
	Events::g().shutdown();
	MH_DisableHook(MH_ALL_HOOKS);
}