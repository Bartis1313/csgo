#pragma once
#include <cstdint>
#include <string>
#include <stdexcept>
#include <functional>
#include <Windows.h>

#include "../ClientMode.hpp"
#include "../CGameMovement.hpp"
#include "../CGlobalVars.hpp"
#include "../CGlowManager.hpp"
#include "../ClientClass.hpp"
#include "../Color.hpp"
#include "../ConVar.hpp"
#include "../CPlayerResource.hpp"
#include "../CUserCmd.hpp"
#include "../Enums.hpp"
#include "../IBaseClientDll.hpp"
#include "../IClientEntityList.hpp"
#include "../ICollideable.hpp"
#include "../ICvar.hpp"
#include "../IEngineTrace.hpp"
#include "../IGameEvent.hpp"
#include "../ILocalize.hpp"
#include "../IMaterialSystem.hpp"
#include "../Input.hpp"
#include "../IPanel.hpp"
#include "../IPrediction.hpp"
#include "../ISurface.hpp"
#include "../IVDebugOverlay.hpp"
#include "../IVEffects.hpp"
#include "../IVEngineClient.hpp"
#include "../IViewRenderBeams.hpp"
#include "../IVModelInfo.hpp"
#include "../IVModelRender.hpp"
#include "../IVRenderView.hpp"
#include "../IVStudioRender.hpp"
#include "../IWeapon.hpp"
#include "../material.hpp"
#include "../Recv.hpp"
#include "../vars.hpp"

#include "../math/Vector.hpp"
#include "../UtlVector.hpp"
#include "../Enums.hpp"
#include "../../utilities/utilities.hpp"

// TODO: rebase features to use selected sdk classes instead of bloat header
//class IPanel;
//class ClientMode;
//class ISurface;
//class IVEngineClient;
//class IBaseClientDLL;
//class IWeapon;
//class IClientEntityList;
//class IClientMode;
//class CClientState;
//class Input;
//class CGlobalVarsBase;
//class IEngineTrace;
//class IMaterialSystem;
//class IVModelInfo;
//class IVDebugOverlay;
//class IGameEventManager2;
//class IViewRender;
//class CGlowManager;
//class CGameMovement;
//class IPrediction;
//class IMoveHelper;
//class ILocalize;
//class ICvar;
//class IConVar;
//class IViewRenderBeams;
//class IVModelRender;
//class IVStudioRender;

namespace interfaces
{
	template <typename T>
	T* getInterface(const char* moduleName, const char* interfaceName)
	{
		// using fun = void* (*)(const char*, int*);
		using fun = void* __cdecl(const char*, int*);
		/*const auto capture = reinterpret_cast<fun>(
			LF(GetProcAddress)(LF(GetModuleHandleA)(moduleName), XOR("CreateInterface"))
			);*/
		const auto capture = reinterpret_cast<std::add_pointer_t<fun>>(
			LF(GetProcAddress)(LF(GetModuleHandleA)(moduleName), XOR("CreateInterface"))
			);
		if (const auto ret = capture(interfaceName, nullptr); ret != nullptr)
			return reinterpret_cast<T*>(ret);
		else
			LF(MessageBoxA)(nullptr, std::format(XOR("Interface {} was nullptr"), interfaceName).c_str(), XOR("Bartis hack"), MB_OK | MB_ICONERROR);

		return nullptr;
	}

	inline ClientMode* clientMode;
	inline IPanel* panel;
	inline ISurface* surface;
	inline IVEngineClient* engine;
	inline IBaseClientDLL* client;
	inline IWeapon* weapon;
	inline IClientEntityList* entList;
	inline Input* input;
	inline CGlobalVarsBase* globalVars;
	inline IEngineTrace* trace;
	inline IMaterialSystem* matSys;
	inline IVModelInfo* modelInfo;
	inline IVDebugOverlay* debugOverlay;
	inline IGameEventManager2* eventManager;
	inline IViewRender* renderView;
	inline CGlowManager* glowManager;
	inline CGameMovement* gameMovement;
	inline IPrediction* prediction;
	inline IMoveHelper* moveHelper;
	inline ILocalize* localize;
	inline ICvar* console;
	inline IConVar* convar;
	inline IViewRenderBeams* beams;
	inline IVModelRender* modelRender;
	inline IVStudioRender* studioRender;
	// remember to dereference!
	inline PlayerResource** resource;

	bool init();
}
