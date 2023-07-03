#pragma once

#include "address.hpp"

#include <SDK/math/Vector.hpp>
#include <SDK/math/matrix.hpp>
#include <SDK/interfaces/ifc.hpp>
#include <SDK/CPlayerResource.hpp>
#include <SDK/CUtlVector.hpp>
#include <utilities/tools/tools.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <Windows.h>
#include <variant>
#include <type_traits>

struct CClientEffectRegistration;
class Player_t;
class AnimationLayer;
class CMoveData;
class ClientMode;
class IViewRenderBeams;
class CGlowManager;
class IWeapon;
class IMoveHelper;
class PlayerResource;
struct IDirect3DDevice9;
class IClientState;
class CGameRules;
class IViewRender;
class CTeslaInfo;
class CEffectData;
class KeyValuesSys;
class IMemAlloc;
class CGlobalVarsBase;
class Input;
class CFlashlightEffect;
class ClientClass;
class IMaterial;
class EnvTonemapController_t;
class FogController_t;
class EnvAmbientLight_t;
struct SDKColor;
struct SDKColorInt;
enum ClassID;
class KeyValues;
class CCommonHostState;
struct decal_t;

using retaddr_t = uintptr_t;

namespace memory
{
	void init();
	void postInit();
};

namespace memory
{
	using loadSky_t = void(__fastcall*)(const char*);
	using findHud_t = uintptr_t * (__thiscall*)(void* /*uintptr_t*/, const char*);
	using sequenceActivity_t = int(__fastcall*)(void*, void*, int);
	using inSmoke_t = bool(__stdcall*)(Vec3, Vec3);
	using isBreakable_t = bool(__thiscall*)(void*);
	using flashlightDestroy_t = void(__thiscall*)(void*, void*);
	using flashlightCreate_t = void* (__thiscall*)(void*, void*, float, float, float, float, int, const char*, float, float);
	using flashlightUpdate_t = void(__thiscall*)(void*, int, const Vec3&, const Vec3&, const Vec3&, const Vec3&, float, float, float, bool, const char*);
	using setAbsOrigin_t = void(__thiscall*)(void*, const Vec3&);
	using setAbsAngle_t = void(__thiscall*)(void*, const Vec3&);
	using isC4Owner_t = bool(__thiscall*)(void*);
	using teslaCreate_t = void(__thiscall*)(CTeslaInfo&);
	using dispatchEffect_t = int(__fastcall*)(const char*, const CEffectData&);
	using particleCached_t = bool(__thiscall*)(void*, const char*);
	using particleFindString_t = void(__thiscall*)(void*, int*, const char*);
	using setParticleControlPoint_t = void(__thiscall*)(void*, int, Vec3*);
	using physicsRunThink_t = bool(__thiscall*)(void*, int);
	using postThinkPhysics_t = bool(__thiscall*)(Player_t*);
	using simulateEntities_t = void(__thiscall*)(Player_t*);
	using loadFromBuffer_t = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, int);
	using keyValuesConstruct_t = void(__thiscall*)(void*, const char*, const char*, int); // or 3rd (2nd without thisptr) arg is int, not sure
	using keyValuesDestruct_t = void(__thiscall*)(void*, int);
	using keyValuesSystem_t = KeyValuesSys * (__cdecl*)();
	using checkThinkFunction_t = void(__thiscall*)(void*, bool);
	using usingStandardWeaponsInVehicle_t = bool(__thiscall*)(void*);
	using selectItem_t = void(__thiscall*)(void*, const char*, int);
	using transferData_t = int(__thiscall*)(void*, const char*, int, void*);
	using reinitPredictables_t = int(__stdcall*)();
	using shutdownPredictables_t = void(__stdcall*)();
	using destroyMaterial_t = void(__thiscall*)(void*);
	using restoreData_t = void(__thiscall*)(Player_t*, const char*, int, int);
	using saveData_t = void(__thiscall*)(Player_t*, const char*, int, int);
	using precipClientThink_t = void(__thiscall*)(void*);
	using precipInit_t = int(__thiscall*)(void*);
	using setAbsVelocity_t = void(__thiscall*)(void*, const Vec3&);
	using getParticleSystemIndex_t = int(__stdcall*)(const char*);
	using dispatchParticleEffect_t = void(__stdcall*)(const char*, Vec3, Vec3, void*, int, void*);
	using renderBoxInternal_t = void(__stdcall*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, SDKColor, IMaterial*, bool);
	using renderLine_t = void(__stdcall*)(const Vec3&, const Vec3&, SDKColor, bool);
	using addGlowBox_t = int(__thiscall*)(void*, Vec3, Vec3, Vec3, Vec3, SDKColor, float);
	using drawModel_t = int(__thiscall*)(void*, int, uint8_t);
	using setString_t = void(__thiscall*)(const char*, const char*);
	using findKey_t = KeyValues*(__thiscall*)(const char*, bool);
	using valveHook_t = char(__cdecl*)(void*, void*, void*, int);
	using valveUnHook_t = char(__cdecl*)(uintptr_t, char);

	inline Address<uintptr_t> traceFilterSimple;
	inline Address<uintptr_t*> returnAddrRadarImage;
	inline Address<Matrix4x4> viewMatrixAddr;
	inline Address<uintptr_t> drawScreenEffectMaterial;
	inline Address<float*> motionBlurVec;
	inline Address<uintptr_t> disableTargetAlloc;
	inline Address<inSmoke_t> throughSmoke;
	inline Address<uintptr_t> smokeCount;
	inline Address<loadSky_t> loadSky;
	inline Address<CClientEffectRegistration*> callbacksHead;
	inline Address<retaddr_t> camThink;
	inline Address<void*> renderDrawPoints;
	inline Address<Player_t**> localPlayer;
	inline Address<void*> csgoHud;
	inline Address<findHud_t> hudfindElement;
	inline Address<uintptr_t> keyValuesFromString;
	inline Address<uintptr_t> animOverlays;
	inline Address<sequenceActivity_t> sequenceActivity;
	inline Address<uintptr_t> cachedBones;
	inline Address<setAbsOrigin_t> setAbsOrigin;
	inline Address<setAbsAngle_t> setAbsAngle;
	inline Address<isC4Owner_t> isC4Owner;
	inline Address<isBreakable_t> isBreakable;
	inline Address<CMoveData*> predictionData;
	inline Address<uintptr_t*> predictionSeed;
	inline Address<flashlightCreate_t> flashlightCreate;
	inline Address<flashlightUpdate_t> flashlightUpdate;
	inline Address<flashlightDestroy_t> flashlightDestroy;
	inline Address<uintptr_t> occlusion;
	inline Address<uintptr_t> velocity;
	inline Address<uintptr_t> accumulate;
	inline Address<particleCached_t> particleIsCached;
	inline Address<void**> particleSystem;
	inline Address<particleFindString_t> particleFindStringIndex;
	inline Address<void*> particleCall;
	inline Address<setParticleControlPoint_t> particleSetControlPoint;
	inline Address<Player_t**> predictedPlayer;
	inline Address<physicsRunThink_t> physicsRunThink;
	inline Address<uintptr_t> lastCommand;
	inline Address<uintptr_t> retAddrToInterpolation;
	inline Address<postThinkPhysics_t> postThinkPhysics;
	inline Address<simulateEntities_t> simulateEntities;
	inline Address<uintptr_t> vecClientImpacts;
	inline Address<loadFromBuffer_t> loadFromBuffer;
	inline Address<keyValuesConstruct_t> keyValuesConstruct;
	inline Address<keyValuesDestruct_t> keyValuesDestruct;
	inline Address<checkThinkFunction_t> checkThinkFunction;
	inline Address<usingStandardWeaponsInVehicle_t> usingStandardWeaponsVehicle;
	inline Address<selectItem_t> selectItem;
	inline Address<transferData_t> transferData;
	inline Address<reinitPredictables_t> reinitPredicatbles;
	inline Address<shutdownPredictables_t> shutdownPredicatbles;
	inline Address<destroyMaterial_t> destroyMaterial;
	inline Address<restoreData_t> restoreData;
	inline Address<saveData_t> saveData;
	inline Address<retaddr_t> allocKeyValuesClient;
	inline Address<retaddr_t> allocKeyValuesEngine;
	inline Address<retaddr_t> flashbangRet;
	inline Address<retaddr_t> flashbangWhiteRet;
	inline Address<precipClientThink_t> precipitationClientThink;
	inline Address<precipInit_t> precipitationInit;
	inline Address<uintptr_t> takeDmg;
	inline Address<setAbsVelocity_t> setAbsVelocity;
	inline Address<uintptr_t> firstMoveChild;
	inline Address<uintptr_t> nextMovePeer;
	inline Address<getParticleSystemIndex_t> getParticleSystemIndex;
	inline Address<dispatchParticleEffect_t> dispatchParticleEffect;
	inline Address<renderBoxInternal_t> renderBoxInternal;
	inline Address<renderLine_t> renderLine;
	inline Address<float*> vignetteBlurStrengthPost;
	inline Address<retaddr_t> scopeDust;
	inline Address<retaddr_t> scopeArc;
	inline Address<addGlowBox_t> addGlowBox;
	inline Address<drawModel_t> baseAnimatingDrawModel;
	inline Address<retaddr_t> renderViewRet;
	inline Address<setString_t> setString;
	inline Address<findKey_t> findKey;
	inline Address<CUtlVector<decal_t*>> decalPool;
	inline Address<valveHook_t> valveHook;
	inline Address<valveUnHook_t> valveUnHook;
	inline Address<retaddr_t> viewFadeSmokeRet;

	inline Address<void*> isUsingPropDebug;
	inline Address<void*> getColorModulation;
	inline Address<void*> extraBonesProcessing;
	inline Address<void*> buildTransformations;
	inline Address<void*> particleSimulate;
	inline Address<void*> sendDataGram;
	inline Address<void*> unkOverviewMap;
	inline Address<void*> isDepth;
	inline Address<void*> fxBlood;
	inline Address<void*> fxBloodSpray;
	inline Address<void*> bloodCallback;
	inline Address<void*> addEnt;
	inline Address<void*> removeEnt;
	inline Address<void*> isFollowedEntity;
	inline Address<void*> spottedEntityUpdate;
	inline Address<void*> fireInternfn;
	inline Address<void*> preRound;
	inline Address<void*> playSoundStep;
	inline Address<void*> bloodUtilCallback;
	inline Address<void*> chudIsHidden;
	inline Address<void*> viewFade;
	inline Address<void*> unkRound;
	inline Address<void*> present;
	inline Address<void*> reset;
	inline Address<void*> updatePostEffects;
	inline Address<void*> getPMaterial;
	inline Address<void*> decalAddToSurface;
	inline Address<void*> createDecal;
	inline Address<void*> drawSmokeFogOverlay;
	inline Address<void*> createParticlePrecip;
	inline Address<void*> initializeParticlePrecip;
	inline Address<void*> viewDrawScene;
	inline Address<void*> drawEffects;
	inline Address<void*> drawWorldAndEntities;
	inline Address<void*> drawTransculentRenderables;

	inline Address<teslaCreate_t> tesla;
	inline Address<dispatchEffect_t> dispatchEffect;

	namespace interfaces
	{
		inline Address<CGlowManager*> glowManager;
		inline Address<IWeapon*> weaponInterface;
		inline Address<PlayerResource*> resourceInterface;
		inline Address<IDirect3DDevice9*> dx9Device;
		inline Address<IClientState*> clientState;
		inline Address<IViewRender*> viewRender;
		inline Address<IMoveHelper*> moveHelper;
		inline Address<IViewRenderBeams*> beams;
		inline Address<KeyValuesSys*> keyValuesSys;
		inline Address<IMemAlloc*> memAlloc;
		inline Address<CGameRules*> gameRules;
		inline Address<CGlobalVarsBase*> globalVars;
		inline Address<ClientMode*> clientMode;
		inline Address<Input*> input;
		inline Address<ClientClass*> preciptation;
		inline Address<EnvTonemapController_t*> toneController;
		inline Address<FogController_t*> fogController;
		inline Address<EnvAmbientLight_t*> ambientLight;
		inline Address<CCommonHostState*> hostState;
	}
}