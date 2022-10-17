#include "memory.hpp"

#include "sigs.hpp"

#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <SDK/math/matrix.hpp>
#include <SDK/CTeslaInfo.hpp>
#include <SDK/CEffectData.hpp>

#include <sstream>
#include <algorithm>
#include <optional>
#include <ranges>

#pragma region modules_names

#define ENGINE_DLL					XOR("engine.dll")
#define CLIENT_DLL					XOR("client.dll")
#define VSTD_DLL					XOR("vstdlib.dll")
#define VGUI_DLL					XOR("vgui2.dll")
#define VGUIMAT_DLL					XOR("vguimatsurface.dll")
#define MATERIAL_DLL				XOR("materialsystem.dll")
#define LOCALIZE_DLL				XOR("localize.dll")
#define STUDIORENDER_DLL			XOR("studiorender.dll")
#define INPUTSYSTEM_DLL				XOR("inputsystem.dll")
#define SHARED_API					XOR("shaderapidx9.dll")
#define TIER_DLL					XOR("tier0.dll")
#define PANORAMA_DLL				XOR("panorama.dll")
#define FILESYS_DLL					XOR("filesystem_stdio.dll")

#pragma endregion


template<typename T>
Memory::Address<T> Memory::Address<T>::initAddr(const std::string& mod, const std::string& sig, uintptr_t offset)
{
	auto [addr, scanned] = g_Memory.scan(mod, sig, offset);
	if (!scanned)
		throw std::runtime_error(FORMAT(XOR("Pattern in {} with sig {} (0x{:X}) was not found"), mod, sig, offset));

	return Address<T>{ addr };
}

std::pair<uintptr_t, bool> Memory::scan(const std::string& mod, const std::string& sig, const uintptr_t offsetToAdd)
{
	std::istringstream iss{ sig };
	std::vector<std::string> parts{ std::istream_iterator<std::string>{ iss }, std::istream_iterator<std::string>{} };
	std::vector<std::optional<std::byte>> actualPattern = {};

	std::for_each(parts.cbegin(), parts.cend(), [&](const std::string& str)
		{
			if (str == "?" || str == "??")
				actualPattern.emplace_back(std::nullopt);
			else
				actualPattern.emplace_back(static_cast<std::byte>(std::stoi(str, nullptr, 16)));
		});

	const auto _module = g_Memory.getModule(mod);
	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(_module);
	const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uint8_t*>(_module) + dosHeader->e_lfanew);
	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	const auto rangeStart = reinterpret_cast<uintptr_t>(_module);

	auto check = [](std::byte* data, const std::vector<std::optional<std::byte>>& _mask)
	{
		for (const auto& _byte : _mask)
		{
			if (_byte && _byte.value() != *data)
				return false;
			data++;
		}
		return true;
	};

	std::pair<uintptr_t, bool> ret = { 0U, false };

	for (auto i : std::views::iota(0U, static_cast<size_t>(sizeOfImage)))
	{
		if (check(reinterpret_cast<std::byte*>(rangeStart + i), actualPattern))
		{
			ret = { rangeStart + i + offsetToAdd, true };
			break;
		}
	}

	m_countedPatterns++;

	return ret;
}

void Memory::init()
{
#define ADD_TO_MAP(name) \
	m_ModulesAddr[name] = (HMODULE)(::li::detail::lazy_module<name##_hash>().cached()); \

	ADD_TO_MAP("engine.dll");
	ADD_TO_MAP("client.dll");
	ADD_TO_MAP("vstdlib.dll");
	ADD_TO_MAP("vgui2.dll");
	ADD_TO_MAP("vguimatsurface.dll");
	ADD_TO_MAP("materialsystem.dll");
	ADD_TO_MAP("localize.dll");
	ADD_TO_MAP("studiorender.dll");
	ADD_TO_MAP("inputsystem.dll");
	ADD_TO_MAP("shaderapidx9.dll");
	ADD_TO_MAP("tier0.dll");
	ADD_TO_MAP("panorama.dll");
	ADD_TO_MAP("filesystem_stdio.dll");

#undef ADD_TO_MAP

	// GENERAL

	m_traceFilterSimple			= m_traceFilterSimple.initAddr(CLIENT_DLL, CTRACE_FILTER_SIMPLE, 0x3D);
	m_returnAddrRadarImage		= m_returnAddrRadarImage.initAddr(PANORAMA_DLL, UNK_FILESYS);
	m_viewMatrixAddr			= m_viewMatrixAddr.initAddr(CLIENT_DLL, VIEW_MATRIX_CLIENT, 0x3).ref().add(0xB0);
	m_drawSpacedRectangle		= m_drawSpacedRectangle.initAddr(CLIENT_DLL, DRAW_SPACE_RECTANGLE_CALL);
	m_motionBlurVec				= m_motionBlurVec.initAddr(CLIENT_DLL, BLUR_MATERIAL_ARR_1, 0x1).ref();
	m_disableTargetAlloc		= m_disableTargetAlloc.initAddr(MATERIAL_DLL, DISABLE_RENDER_TARGET_ALLOC, 0x2);
	m_throughSmoke				= m_throughSmoke.initAddr(CLIENT_DLL, GOES_THROUGH_SMOKE);
	m_smokeCount				= m_throughSmoke.add(0x8).ref().cast<uintptr_t>();
	m_loadSky					= m_loadSky.initAddr(ENGINE_DLL, LOAD_SKY);
	m_callbacksHead				= m_callbacksHead.initAddr(CLIENT_DLL, HEAD_OF_EFFECTS, 0x2).ref(Dereference::TWICE);
	m_camThink					= m_camThink.initAddr(CLIENT_DLL, CAM_THINK);
	m_renderDrawPoints			= m_renderDrawPoints.initAddr(STUDIORENDER_DLL, R_STUDIODRAWPOINTS);
	m_localPlayer				= m_localPlayer.initAddr(CLIENT_DLL, LOCAL_PLAYER, 0x2).ref();
	m_csgoHud					= m_csgoHud.initAddr(CLIENT_DLL, CSGO_HUD, 0x1).ref();
	m_hudfindElement			= m_hudfindElement.initAddr(CLIENT_DLL, FIND_ELEMENT);
	m_keyValuesFromString		= m_keyValuesFromString.initAddr(CLIENT_DLL, KEY_VALUES_FROM_STR);
	m_animOverlays				= m_animOverlays.initAddr(CLIENT_DLL, ANIMATION_LAYER, 0x2).ref();
	m_sequenceActivity			= m_sequenceActivity.initAddr(CLIENT_DLL, SEQUENCE_ACTIVITY);
	m_cachedBones				= m_cachedBones.initAddr(CLIENT_DLL, CACHED_BONE, 0x2).ref().add(0x4);
	m_setAbsOrigin				= m_setAbsOrigin.initAddr(CLIENT_DLL, SETABSORIGIN);
	m_isC4Owner					= m_isC4Owner.initAddr(CLIENT_DLL, HASC4);
	m_isBreakable				= m_isBreakable.initAddr(CLIENT_DLL, IS_BREAKBLE);
	m_predictionData			= m_predictionData.initAddr(CLIENT_DLL, PREDICTION_MOVE_DATA, 0x1).ref(Dereference::TWICE);
	m_predictionSeed			= m_predictionSeed.initAddr(CLIENT_DLL, PREDICTIONRANDOMSEED, 0x2).ref();
	m_flashlightCreate			= m_flashlightCreate.initAddr(CLIENT_DLL, FLASHLIGHT_CREATE);
	m_flashlightUpdate			= m_flashlightUpdate.initAddr(CLIENT_DLL, FLASHLIGHT_UPDATE);
	m_flashlightDestroy			= m_flashlightDestroy.initAddr(CLIENT_DLL, FLASHLIGHT_DESTROY);
	m_occlusion					= m_occlusion.initAddr(CLIENT_DLL, SETUP_OCCLUSION);
	m_velocity					= m_velocity.initAddr(CLIENT_DLL, SETUP_VELOCITY);
	m_accumulate				= m_accumulate.initAddr(CLIENT_DLL, SETUP_VELOCITY);
	m_particleIsCached			= m_particleIsCached.initAddr(CLIENT_DLL, IS_EFFECT_CACHED);
	m_particleSystem			= m_particleSystem.initAddr(CLIENT_DLL, PARTICLE_SYSTEM, 0x7).ref(Dereference::TWICE);
	m_particleFindStringIndex	= m_particleFindStringIndex.initAddr(CLIENT_DLL, FIND_STRING_INDEX);
	m_particleCall				= m_particleCall.initAddr(CLIENT_DLL, PARTICLE_CALL);
	m_particleSetControlPoint	= m_particleSetControlPoint.initAddr(CLIENT_DLL, SET_PARTICLE_POINT);

	// SDK INTERFACES

	m_beams						= m_beams.initAddr(CLIENT_DLL, BEAMS, 0x1).ref();
	m_glowManager				= m_glowManager.initAddr(CLIENT_DLL, GLOWMANAGER, 0x3).ref();
	m_weaponInterface			= m_weaponInterface.initAddr(CLIENT_DLL, WEAPONDATA, 0x2).ref();
	m_moveHelper				= m_moveHelper.initAddr(CLIENT_DLL, MOVEHELPER, 0x2).ref(Dereference::TWICE);
	m_resourceInterface			= m_resourceInterface.initAddr(CLIENT_DLL, PLAYER_RESOURCE, 0x4).ref();
	m_dx9Device					= m_dx9Device.initAddr(SHARED_API, DX9_DEVICE, 0x1).ref(Dereference::TWICE);
	m_clientState				= m_clientState.initAddr(ENGINE_DLL, CLIENT_STATE, 0x1).ref(Dereference::TWICE);
	m_gameRules					= m_gameRules.initAddr(CLIENT_DLL, GAME_RULES, 0x1).ref();
	m_viewRender				= m_viewRender.initAddr(CLIENT_DLL, VIEW_RENDER, 0x2).ref(Dereference::TWICE);

	// HOOKS

	m_clientValidAddr			= m_clientValidAddr.initAddr(CLIENT_DLL, NEW_CHECK);
	m_enginevalidAddr			= m_enginevalidAddr.initAddr(ENGINE_DLL, NEW_CHECK);
	m_studioRenderValidAddr		= m_studioRenderValidAddr.initAddr(STUDIORENDER_DLL, NEW_CHECK);
	m_materialSysValidAddr		= m_materialSysValidAddr.initAddr(MATERIAL_DLL, NEW_CHECK);
	m_isUsingPropDebug			= m_isUsingPropDebug.initAddr(ENGINE_DLL, IS_USING_PROP_DEBUG);
	m_getColorModulation		= m_getColorModulation.initAddr(MATERIAL_DLL, GET_COLOR_MODULATION);
	m_extraBonesProcessing		= m_extraBonesProcessing.initAddr(CLIENT_DLL, EXTRA_BONES_PROCCESSING);
	m_buildTransformations		= m_buildTransformations.initAddr(CLIENT_DLL, BUILD_TRANSFORMATIONS);
	m_particleSimulate			= m_particleSimulate.initAddr(CLIENT_DLL, PARTICLE_SIMULATE);
	m_sendDataGram				= m_sendDataGram.initAddr(ENGINE_DLL, SEND_DATAGRAM);
	m_unkOverviewMap			= m_unkOverviewMap.initAddr(CLIENT_DLL, UNK_OVERVIEWMAP);
	m_isDepth					= m_isDepth.initAddr(CLIENT_DLL, IS_DEPTH);
	m_fxBlood					= m_fxBlood.initAddr(CLIENT_DLL, FX_BLOOD);

	// REST

	m_tesla						= m_tesla.initAddr(CLIENT_DLL, FX_TESLA);
	m_dispatchEffect			= m_dispatchEffect.initAddr(CLIENT_DLL, DISPATCH_EFFECT);

	LOG_INFO(XOR("memory init success, {} sigs done"), m_countedPatterns);
}
