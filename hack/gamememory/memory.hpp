#pragma once

#include <SDK/math/Vector.hpp>
#include <SDK/math/matrix.hpp>
#include <SDK/interfaces/ifc.hpp>
#include <SDK/CPlayerResource.hpp>
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

enum ClassID;

using retaddr_t = uintptr_t;

namespace memory
{
	enum class Dereference : size_t
	{
		ONCE = 1,
		TWICE,
		THREE
	};

	void init();
	void postInit();

	namespace detail
	{
		inline std::unordered_map<std::string_view, HMODULE> m_ModulesAddr;
	}

	inline HMODULE getModule(const std::string_view str) { return detail::m_ModulesAddr.at(str); }
	template<typename T, li::detail::offset_hash_pair __hash>
	T exportVar(const std::string_view _module)
	{
		return static_cast<T>(::li::detail::lazy_function<__hash, T>().in(getModule(_module)));
	}	

	template<typename T>
	struct Address
	{
		constexpr Address() = default;
		// pass by offset
		constexpr Address(uintptr_t addr) :
			m_addr{ addr }
		{}
		constexpr Address(void* addr) :
			m_addr{ reinterpret_cast<uintptr_t>(addr) }
		{}

		// raw place in memory as offset
		constexpr auto getRawAddr() const { return m_addr; }
		// cast to anything
		template<typename K>
		constexpr auto cast() const { return Address<K>{ m_addr }; }
		constexpr auto add(uintptr_t extraOffset) const { return Address{ m_addr + extraOffset }; }
		constexpr auto sub(uintptr_t extraOffset) const { return Address{ m_addr - extraOffset }; }
		// dereference x times. Possible args are: 1, 2, 3. There will for sure won't be a case for 4 level dereference. 3rd is very rare.
		constexpr auto deRef(Dereference times = Dereference::ONCE)
		{
			for ([[maybe_unused]] auto i : std::views::iota(0U, E2T(times)))
				m_addr = *reinterpret_cast<uintptr_t*>(m_addr);

			return Address<T>{ m_addr };
		}
		// get as rel32
		constexpr auto rel(uintptr_t relOffset = 0x1, uintptr_t absOffset = 0x0) const
		{
			const auto jump = m_addr + relOffset;
			const auto target = *reinterpret_cast<decltype(jump)*>(jump);
			return Address<T>{ jump + absOffset + 0x4 + target };
		}
		// will work for classes types too
		constexpr T operator()() const
		{
			if constexpr (std::is_class_v<T>)
				return *reinterpret_cast<T*>(m_addr);
			else
				return (T)(m_addr);
		}

		constexpr T operator->() const
		{
			if constexpr (std::is_class_v<T>)
				return *reinterpret_cast<T*>(m_addr);
			else
				return (T)(m_addr);
		}

		template<size_t N>
		Address<T> scan(const std::string_view mod, const std::array<std::optional<uint8_t>, N>& sig);
		template<li::detail::offset_hash_pair hash>
		Address<T> byExport(const std::string_view module);
		template<typename TT>
		Address<T> byVFunc(const Interface<TT>& ifc, size_t index);
		// static pointer
		Address<T> findFromGame(ClassID id);
		// anything that changes every round
		Address<T> findFromGameLoop(ClassID id);

		using value = T;
		uintptr_t m_addr;
		std::string_view m_module;
	};
};

namespace memory
{
	using loadSky_t = void(__fastcall*)(const char*);
	using findHud_t = uintptr_t * (__thiscall*)(void* /*uintptr_t*/, const char*);
	using sequenceActivity_t = int(__fastcall*)(void*, void*, int);
	using inSmoke_t = bool(__cdecl*)(Vec3, Vec3);
	using isBreakable_t = bool(__thiscall*)(void*);
	using flashlightDestroy_t = void(__thiscall*)(void*, void*);
	using flashlightCreate_t = void* (__thiscall*)(void*, void*, float, float, float, float, int, const char*, float, float);
	using flashlightUpdate_t = void(__thiscall*)(void*, int, const Vec3&, const Vec3&, const Vec3&, const Vec3&, float, float, float, bool, const char*);
	using setAbsOrigin_t = void(__thiscall*)(void*, const Vec3&);
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
	inline Address<void*> camThink;
	inline Address<void*> renderDrawPoints;
	inline Address<Player_t**> localPlayer;
	inline Address<void*> csgoHud;
	inline Address<findHud_t> hudfindElement;
	inline Address<uintptr_t> keyValuesFromString;
	inline Address<uintptr_t> animOverlays;
	inline Address<sequenceActivity_t> sequenceActivity;
	inline Address<uintptr_t> cachedBones;
	inline Address<setAbsOrigin_t> setAbsOrigin;
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
	}
}