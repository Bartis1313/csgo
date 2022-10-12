#pragma once

#include <SDK/math/Vector.hpp>
#include <SDK/math/matrix.hpp>

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

class Memory
{
public:
	enum class Dereference : size_t
	{
		ONCE = 1,
		TWICE,
		THREE
	};

	virtual void init();

	template<typename T>
	struct Address
	{
	public:
		constexpr Address() = default;
		// pass by offset
		constexpr Address(uintptr_t addr) :
			m_addr{ addr }
		{}
		constexpr Address(uintptr_t* addr) :
			m_addr{ reinterpret_cast<uintptr_t>(addr) }
		{}
		constexpr Address(void* addr) :
			m_addr{ reinterpret_cast<uintptr_t>(addr) }
		{}

		// raw place in memory as offset
		constexpr uintptr_t getRawAddr() const { return m_addr; }
		// cast to anything
		template<typename K>
		constexpr auto cast() const { return Address<K>{ m_addr }; }
		// wrapper for sig scan, when error it throws one
		Address<T> initAddr(const std::string& mod, const std::string& sig, uintptr_t offset = 0);
		// add bytes, useful for creating "chains" with ref(), depends on use case.
		constexpr Address<T> add(uintptr_t extraOffset) const { return Address{ m_addr + extraOffset }; }
		// dereference x times. Possible args are: 1, 2, 3. There will for sure won't be a case for 4 level dereference. 3rd is very rare.
		constexpr auto ref(Dereference times = Dereference::ONCE) const
		{
			auto addr = m_addr;

			for ([[maybe_unused]] auto i : std::views::iota(0U, E2T(times)))
				addr = *reinterpret_cast<uintptr_t*>(addr);

			return Address<T>{ addr };
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
	private:
		uintptr_t m_addr;
	};
private:
	using loadSky_t = void(__fastcall*)(const char*);
	using findHud_t = uintptr_t*(__thiscall*)(void* /*uintptr_t*/, const char*);
	using sequenceActivity_t = int(__fastcall*)(void*, void*, int);
	using inSmoke_t = bool(__cdecl*)(Vec3, Vec3);
	using isBreakable_t = bool(__thiscall*)(void*);
	using flashlightDestroy_t = void(__thiscall*)(void*, void*);
	using flashlightUpdate_t =  void(__thiscall*)(void*, int, const Vec3&, const Vec3&, const Vec3&, const Vec3&, float, float, float, bool, const char*);
	using setAbsOrigin_t = void(__thiscall*)(void*, const Vec3&);
	using isC4Owner_t = bool(__thiscall*)(void*);
	using teslaCreate_t = void(__thiscall*)(CTeslaInfo&);
	using dispatchEffect_t = int(__fastcall*)(const char*, const CEffectData&);
	using particleCached_t = bool(__thiscall*)(void*, const char*);
	using particleFindString_t = void(__thiscall*)(void*, int*, const char*);
	using setParticleControlPoint_t = void(__thiscall*)(void*, int, Vec3*);


	std::pair<uintptr_t, bool> scan(const std::string& mod, const std::string& sig, const uintptr_t offsetToAdd = 0);

	std::unordered_map<std::string_view, HMODULE> m_ModulesAddr;
	// for info
	size_t m_countedPatterns = 0;
public:
	HMODULE getModule(const std::string_view str) { return m_ModulesAddr.at(str); }

	Address<uintptr_t> m_traceFilterSimple;
	Address<uintptr_t*> m_returnAddrRadarImage;
	Address<Matrix4x4> m_viewMatrixAddr;
	Address<uintptr_t> m_drawSpacedRectangle;
	Address<float*> m_motionBlurVec;
	Address<uintptr_t> m_disableTargetAlloc;
	Address<inSmoke_t> m_throughSmoke;
	Address<uintptr_t> m_smokeCount;
	Address<loadSky_t> m_loadSky;
	Address<CClientEffectRegistration*> m_callbacksHead;
	Address<void*> m_camThink;
	Address<void*> m_renderDrawPoints;
	Address<Player_t**> m_localPlayer;
	Address<void*> m_csgoHud;
	Address<findHud_t> m_hudfindElement;
	Address<uintptr_t> m_keyValuesFromString;
	Address<uintptr_t> m_animOverlays;
	Address<sequenceActivity_t> m_sequenceActivity;
	Address<uintptr_t> m_cachedBones;
	Address<setAbsOrigin_t> m_setAbsOrigin;
	Address<isC4Owner_t> m_isC4Owner;
	Address<isBreakable_t> m_isBreakable;
	Address<CMoveData*> m_predictionData;
	Address<uintptr_t*> m_predictionSeed;
	Address<void*> m_flashlightCreate;
	Address<flashlightUpdate_t> m_flashlightUpdate;
	Address<flashlightDestroy_t> m_flashlightDestroy;
	Address<uintptr_t> m_occlusion;
	Address<uintptr_t> m_velocity;
	Address<uintptr_t> m_accumulate;
	Address<particleCached_t> m_particleIsCached;
	Address<void**> m_particleSystem;
	Address<particleFindString_t> m_particleFindStringIndex;
	Address<void*> m_particleCall;
	Address<setParticleControlPoint_t> m_particleSetControlPoint;

	Address<IViewRenderBeams*> m_beams;
	Address<CGlowManager*> m_glowManager;
	Address<IWeapon*> m_weaponInterface;
	Address<IMoveHelper*> m_moveHelper;
	Address<PlayerResource**> m_resourceInterface;
	Address<IDirect3DDevice9*> m_dx9Device;
	Address<IClientState*> m_clientState;
	Address<CGameRules*> m_gameRules;
	Address<IViewRender*> m_viewRender;

	Address<void*> m_clientValidAddr;
	Address<void*> m_enginevalidAddr;
	Address<void*> m_studioRenderValidAddr;
	Address<void*> m_materialSysValidAddr;
	Address<void*> m_isUsingPropDebug;
	Address<void*> m_getColorModulation;
	Address<void*> m_extraBonesProcessing;
	Address<void*> m_buildTransformations;
	Address<void*> m_particleSimulate;
	Address<void*> m_sendDataGram;
	Address<void*> m_unkOverviewMap;
	Address<void*> m_isDepth;
	Address<void*> m_fxBlood;

	Address<teslaCreate_t> m_tesla;
	Address<dispatchEffect_t> m_dispatchEffect;
};

[[maybe_unused]] inline auto g_Memory = Memory{};