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
class World_t;
class Entity_t;
class CascadeLight_t;
class Precipitation_t;
enum ParticleAttachment_t;
class CNewParticleEffect;

using retaddr_t = uintptr_t;

namespace memory
{
	HACK_INIT void init();
	void postInit();
};

namespace memory
{
	using loadSky_t = void(HACK_FASTCALL*)(const char*);
	using findHud_t = uintptr_t * (HACK_THISCALL*)(void* /*uintptr_t*/, const char*);
	using sequenceActivity_t = int(HACK_FASTCALL*)(void*, void*, int);
	using inSmoke_t = bool(HACK_CDECL*)(Vec3, Vec3);
	using isBreakable_t = bool(HACK_THISCALL*)(void*);
	using flashlightDestroy_t = void(HACK_THISCALL*)(void*, void*);
	using flashlightCreate_t = void* (HACK_THISCALL*)(void*, void*, float, float, float, float, int, const char*, float, float);
	using flashlightUpdate_t = void(HACK_THISCALL*)(void*, int, const Vec3&, const Vec3&, const Vec3&, const Vec3&, float, float, float, bool, const char*);
	using setAbsOrigin_t = void(HACK_THISCALL*)(void*, const Vec3&);
	using setAbsAngle_t = void(HACK_THISCALL*)(void*, const Vec3&);
	using isC4Owner_t = bool(HACK_THISCALL*)(void*);
	using teslaCreate_t = void(HACK_THISCALL*)(CTeslaInfo&);
	using dispatchEffect_t = int(HACK_FASTCALL*)(const char*, const CEffectData&);
	using particleCached_t = bool(HACK_THISCALL*)(void*, const char*);
	using particleFindString_t = void(HACK_THISCALL*)(void*, int*, const char*);
	using setParticleControlPoint_t = void(HACK_THISCALL*)(void*, int, Vec3*);
	using physicsRunThink_t = bool(HACK_THISCALL*)(void*, int);
	using postThinkPhysics_t = bool(HACK_THISCALL*)(Player_t*);
	using simulateEntities_t = void(HACK_THISCALL*)(Player_t*);
	using loadFromBuffer_t = void(HACK_THISCALL*)(void*, const char*, const char*, void*, const char*, void*, int);
	using keyValuesConstruct_t = void(HACK_THISCALL*)(void*, const char*, const char*, int); // or 3rd (2nd without thisptr) arg is int, not sure
	using keyValuesDestruct_t = void(HACK_THISCALL*)(void*, int);
	using keyValuesSystem_t = KeyValuesSys * (HACK_CDECL*)();
	using checkThinkFunction_t = void(HACK_THISCALL*)(void*, bool);
	using usingStandardWeaponsInVehicle_t = bool(HACK_THISCALL*)(void*);
	using selectItem_t = void(HACK_THISCALL*)(void*, const char*, int);
	using transferData_t = int(HACK_THISCALL*)(void*, const char*, int, void*);
	using reinitPredictables_t = int(HACK_STDCALL*)();
	using shutdownPredictables_t = void(HACK_STDCALL*)();
	using destroyMaterial_t = void(HACK_THISCALL*)(void*);
	using restoreData_t = void(HACK_THISCALL*)(Player_t*, const char*, int, int);
	using saveData_t = void(HACK_THISCALL*)(Player_t*, const char*, int, int);
	using precipClientThink_t = void(HACK_THISCALL*)(void*);
	using precipInit_t = int(HACK_THISCALL*)(void*);
	using setAbsVelocity_t = void(HACK_THISCALL*)(void*, const Vec3&);
	using getParticleSystemIndex_t = int(HACK_STDCALL*)(const char*);
	using dispatchParticleEffect_t = void(HACK_STDCALL*)(const char*, const Vec3&, const Vec3&, ParticleAttachment_t, void*, int);
	using renderBoxInternal_t = void(HACK_STDCALL*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, SDKColor, IMaterial*, bool);
	using renderLine_t = void(HACK_STDCALL*)(const Vec3&, const Vec3&, SDKColor, bool);
	using addGlowBox_t = int(HACK_THISCALL*)(void*, Vec3, Vec3, Vec3, Vec3, SDKColor, float);
	using drawModel_t = int(HACK_THISCALL*)(void*, int, uint8_t);
	using setString_t = void(HACK_THISCALL*)(void*, const char*, const char*);
	using findKey_t = KeyValues*(HACK_THISCALL*)(void*, const char*, bool);
	using valveHook_t = char(HACK_CDECL*)(void*, void*, void*, int);
	using valveUnHook_t = char(HACK_CDECL*)(uintptr_t, char);
	using precipDestruct_t = void(HACK_THISCALL*)(void*);
	using stopSound_t = void(HACK_THISCALL*)(void*, const char*);
	using newParticleCreate_t = CNewParticleEffect*(HACK_STDCALL*)(const char*, ParticleAttachment_t, int, Vec3, Matrix3x4*);
	using newParticleSetControlPoint_t = void(HACK_THISCALL*)(void*, int, const Vec3&);
	using newParticleSetControlPointEnt_t = void(HACK_THISCALL*)(void*, int, void*);
	using restoreMaterialSystemObjects_t = void(HACK_CDECL*)(int);
	using restoreMaterialSystemObjectsMdl_t = void(HACK_THISCALL*)(void*, int);
	using releaseMaterialSystemObjects_t = void(HACK_CDECL*)();
	using forceSingleThreaded_t = void(HACK_THISCALL*)(void*);
	using restoreShaderObjects_t = void(HACK_THISCALL*)(void*, int, void*, int);
	using releaseShaderObjects_t = void(HACK_THISCALL*)(void*, int);
	using precacheVars_t = bool(HACK_THISCALL*)(void*, void*, void*, void*);

	inline Address<uintptr_t> traceFilterSimple;
	inline Address<uintptr_t*> returnAddrRadarImage;
	inline Address<Matrix4x4> viewMatrixAddr;
	inline Address<uintptr_t> drawScreenEffectMaterial;
	inline Address<float*> motionBlurVec;
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
	inline Address<CUtlVector<Precipitation_t*>> gPrecipitations;
	inline Address<precipDestruct_t> precipDestruct;
	inline Address<stopSound_t> stopSound;
	inline Address<retaddr_t> particleGetVCollideRet;
	inline Address<newParticleCreate_t> newParticleCreate;
	inline Address<newParticleSetControlPoint_t> newParticleSetControlPoint;
	inline Address<newParticleSetControlPointEnt_t> newParticleSetControlPointEnt;
	inline Address<retaddr_t> setControlPointParticleWeatherRet;
	inline Address<uintptr_t> vPhysicsGetObject;
	inline Address<restoreMaterialSystemObjects_t> restoreMaterialSystemObjects;
	inline Address<forceSingleThreaded_t> forceSingleThreaded;
	inline Address<precacheVars_t> precacheVars;
	inline Address<char*> levelName; // sizeof here [actual address in first opcode] E8 ? ? ? ? 83 C4 08 83 3E 0A

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
	inline Address<void*> clientCsNormalEvent;
	inline Address<void*> tracerDraw;
	inline Address<void*> dispatchInnerParticlePrecip;
	inline Address<void*> getVelocity;

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
		inline Address<World_t*> gameWorld;
		inline Address<ClientClass*> dynamicLight;
	}
}