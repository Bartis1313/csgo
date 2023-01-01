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

enum ClassID;

using retaddr_t = uintptr_t;

class Memory
{
public:
	enum class Dereference : size_t
	{
		ONCE = 1,
		TWICE,
		THREE
	};

	void init();
	void postInit();

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

		Address<T> scan(const std::string_view mod, const std::string_view sig, uintptr_t offset = 0);
		template<li::detail::hash_t::value_type hash>
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
private:
	std::unordered_map<std::string_view, HMODULE> m_ModulesAddr;
public:
	HMODULE getModule(const std::string_view str) { return m_ModulesAddr.at(str); }
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

	inline Memory::Address<uintptr_t> traceFilterSimple;
	inline Memory::Address<uintptr_t*> returnAddrRadarImage;
	inline Memory::Address<Matrix4x4> viewMatrixAddr;
	inline Memory::Address<uintptr_t> drawSpacedRectangle;
	inline Memory::Address<float*> motionBlurVec;
	inline Memory::Address<uintptr_t> disableTargetAlloc;
	inline Memory::Address<inSmoke_t> throughSmoke;
	inline Memory::Address<uintptr_t> smokeCount;
	inline Memory::Address<loadSky_t> loadSky;
	inline Memory::Address<CClientEffectRegistration*> callbacksHead;
	inline Memory::Address<void*> camThink;
	inline Memory::Address<void*> renderDrawPoints;
	inline Memory::Address<Player_t**> localPlayer;
	inline Memory::Address<void*> csgoHud;
	inline Memory::Address<findHud_t> hudfindElement;
	inline Memory::Address<uintptr_t> keyValuesFromString;
	inline Memory::Address<uintptr_t> animOverlays;
	inline Memory::Address<sequenceActivity_t> sequenceActivity;
	inline Memory::Address<uintptr_t> cachedBones;
	inline Memory::Address<setAbsOrigin_t> setAbsOrigin;
	inline Memory::Address<isC4Owner_t> isC4Owner;
	inline Memory::Address<isBreakable_t> isBreakable;
	inline Memory::Address<CMoveData*> predictionData;
	inline Memory::Address<uintptr_t*> predictionSeed;
	inline Memory::Address<flashlightCreate_t> flashlightCreate;
	inline Memory::Address<flashlightUpdate_t> flashlightUpdate;
	inline Memory::Address<flashlightDestroy_t> flashlightDestroy;
	inline Memory::Address<uintptr_t> occlusion;
	inline Memory::Address<uintptr_t> velocity;
	inline Memory::Address<uintptr_t> accumulate;
	inline Memory::Address<particleCached_t> particleIsCached;
	inline Memory::Address<void**> particleSystem;
	inline Memory::Address<particleFindString_t> particleFindStringIndex;
	inline Memory::Address<void*> particleCall;
	inline Memory::Address<setParticleControlPoint_t> particleSetControlPoint;
	inline Memory::Address<Player_t**> predictedPlayer;
	inline Memory::Address<physicsRunThink_t> physicsRunThink;
	inline Memory::Address<uintptr_t> lastCommand;
	inline Memory::Address<uintptr_t> retAddrToInterpolation;
	inline Memory::Address<postThinkPhysics_t> postThinkPhysics;
	inline Memory::Address<simulateEntities_t> simulateEntities;
	inline Memory::Address<uintptr_t> vecClientImpacts;
	inline Memory::Address<loadFromBuffer_t> loadFromBuffer;
	inline Memory::Address<keyValuesConstruct_t> keyValuesConstruct;
	inline Memory::Address<keyValuesDestruct_t> keyValuesDestruct;
	inline Memory::Address<checkThinkFunction_t> checkThinkFunction;
	inline Memory::Address<usingStandardWeaponsInVehicle_t> usingStandardWeaponsVehicle;
	inline Memory::Address<selectItem_t> selectItem;
	inline Memory::Address<transferData_t> transferData;
	inline Memory::Address<reinitPredictables_t> reinitPredicatbles;
	inline Memory::Address<shutdownPredictables_t> shutdownPredicatbles;

	inline Memory::Address<void*> clientValidAddr;
	inline Memory::Address<void*> enginevalidAddr;
	inline Memory::Address<void*> studioRenderValidAddr;
	inline Memory::Address<void*> materialSysValidAddr;
	inline Memory::Address<void*> isUsingPropDebug;
	inline Memory::Address<void*> getColorModulation;
	inline Memory::Address<void*> extraBonesProcessing;
	inline Memory::Address<void*> buildTransformations;
	inline Memory::Address<void*> particleSimulate;
	inline Memory::Address<void*> sendDataGram;
	inline Memory::Address<void*> unkOverviewMap;
	inline Memory::Address<void*> isDepth;
	inline Memory::Address<void*> fxBlood;
	inline Memory::Address<void*> addEnt;
	inline Memory::Address<void*> removeEnt;
	inline Memory::Address<void*> isFollowedEntity;
	inline Memory::Address<void*> spottedEntityUpdate;
	inline Memory::Address<void*> fireInternfn;
	inline Memory::Address<void*> preRound;
	inline Memory::Address<void*> playSoundStep;

	inline Memory::Address<teslaCreate_t> tesla;
	inline Memory::Address<dispatchEffect_t> dispatchEffect;

	namespace interfaces
	{
		inline Memory::Address<CGlowManager*> glowManager;
		inline Memory::Address<IWeapon*> weaponInterface;
		inline Memory::Address<PlayerResource*> resourceInterface;
		inline Memory::Address<IDirect3DDevice9*> dx9Device;
		inline Memory::Address<IClientState*> clientState;
		inline Memory::Address<IViewRender*> viewRender;
		inline Memory::Address<IMoveHelper*> moveHelper;
		inline Memory::Address<IViewRenderBeams*> beams;
		inline Memory::Address<KeyValuesSys*> keyValuesSys;
		inline Memory::Address<IMemAlloc*> memAlloc;
		inline Memory::Address<CGameRules*> gameRules;
		inline Memory::Address<CGlobalVarsBase*> globalVars;
		inline Memory::Address<ClientMode*> clientMode;
		inline Memory::Address<Input*> input;
		inline Memory::Address<ClientClass*> preciptation;
	}
}

[[maybe_unused]] inline auto g_Memory = Memory{};