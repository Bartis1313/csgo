#include "memory.hpp"

#include "sigs.hpp"
#include "modules.hpp"
#include "pattern.hpp"

#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <SDK/math/matrix.hpp>
#include <SDK/CTeslaInfo.hpp>
#include <SDK/CEffectData.hpp>
#include <SDK/IPrediction.hpp>
#include <SDK/CGameRules.hpp>
#include <SDK/Input.hpp>
#include <SDK/IViewRenderBeams.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/game.hpp>

#include <sstream>
#include <algorithm>
#include <optional>
#include <ranges>

template<typename T>
memory::Address<T> memory::Address<T>::scan(const std::string_view mod, const std::string_view sig, uintptr_t offset)
{
	const auto _module = memory::getModule(mod);
	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(_module);
	const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uint8_t*>(_module) + dosHeader->e_lfanew);
	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	const auto rangeStart = reinterpret_cast<uintptr_t>(_module);

	// tried boyer_moore_horspool_searcher, it would need some big edits to work
	/*const auto converted = SigConvert<std::byte>::get(sig);
	const auto searched = std::ranges::search(reinterpret_cast<std::byte*>(_module), reinterpret_cast<std::byte*>(_module) + sizeOfImage, converted.begin(), converted.end());


	if (std::ranges::begin(searched) != reinterpret_cast<std::byte*>(_module) + sizeOfImage)
	{
		m_addr = reinterpret_cast<uintptr_t>(std::ranges::begin(searched)) + offset;
		m_module = mod;
		return *this;
	}
	*/

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

	const auto converted = SigConvert<std::byte>::get(sig);

	for (auto i : std::views::iota(reinterpret_cast<size_t>(_module), static_cast<size_t>(sizeOfImage + reinterpret_cast<size_t>(_module))))
	{
		if (check(reinterpret_cast<std::byte*>(i), converted))
		{
			m_addr = i + offset;
			break;
		}
	}

	assert(m_addr);
	m_module = mod;
	return Address<T>{ m_addr };
}

template<typename T>
template<li::detail::offset_hash_pair hash>
memory::Address<T> memory::Address<T>::byExport(const std::string_view _module)
{
	m_addr = exportVar<decltype(m_addr), hash>(_module);

	return Address<T>{ m_addr };
}

template<typename T>
template<typename TT>
memory::Address<T> memory::Address<T>::byVFunc(const Interface<TT>& ifc, size_t index)
{
	m_addr = reinterpret_cast<uintptr_t>(vfunc::getVFunc(reinterpret_cast<void*>(ifc.base), index));

	return Address<T>{ m_addr };
}

#include <SDK/IBaseClientDll.hpp>
#include <SDK/ClientClass.hpp>

template<typename T>
memory::Address<T> memory::Address<T>::findFromGame(ClassID id)
{
	for (auto _class = memory::interfaces::client->getAllClasses(); _class; _class = _class->m_next)
	{
		if (_class->m_classID == id)
			return Address<T>{ _class };
	}

	return Address<T>{ 0U };
}

#include <SDK/IVEngineClient.hpp>

template<typename T>
memory::Address<T> memory::Address<T>::findFromGameLoop(ClassID id)
{
	for (auto i : std::views::iota(memory::interfaces::engine->getMaxClients(), memory::interfaces::entList->getHighestIndex() + 1))
	{
		auto entity = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientEntity(i));
		if (!entity)
			continue;

		auto idx = entity->clientClass()->m_classID;
		if (idx == id)
			return Address<T>{ entity };
	}

	return Address<T>{ 0U };
}

//#include <deps/fixed.hpp>

void memory::init()
{
#define ADD_TO_MAP(name) \
	detail::m_ModulesAddr[name] =  reinterpret_cast<HMODULE>(LI_MODULE(name).cached()); \

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
	ADD_TO_MAP("datacache.dll");
	ADD_TO_MAP("server.dll");

#undef ADD_TO_MAP

	using namespace memory;
	using namespace memory::interfaces;

	traceFilterSimple = traceFilterSimple.scan(CLIENT_DLL, CTRACE_FILTER_SIMPLE, 0x3D);
	returnAddrRadarImage = returnAddrRadarImage.scan(PANORAMA_DLL, UNK_FILESYS);
	viewMatrixAddr = viewMatrixAddr.scan(CLIENT_DLL, VIEW_MATRIX_CLIENT, 0x3).deRef().add(0xB0);
	drawSpacedRectangle = drawSpacedRectangle.scan(CLIENT_DLL, DRAW_SPACE_RECTANGLE_CALL);
	motionBlurVec = motionBlurVec.scan(CLIENT_DLL, BLUR_MATERIAL_ARR_1, 0x1).deRef();
	disableTargetAlloc = disableTargetAlloc.scan(MATERIAL_DLL, DISABLE_RENDER_TARGET_ALLOC, 0x2);
	throughSmoke = throughSmoke.scan(CLIENT_DLL, GOES_THROUGH_SMOKE);
	smokeCount = throughSmoke.add(0x8).deRef().cast<uintptr_t>();
	loadSky = loadSky.scan(ENGINE_DLL, LOAD_SKY);
	callbacksHead = callbacksHead.scan(CLIENT_DLL, HEAD_OF_EFFECTS, 0x2).deRef(Dereference::TWICE);
	camThink = camThink.scan(CLIENT_DLL, CAM_THINK);
	renderDrawPoints = renderDrawPoints.scan(STUDIORENDER_DLL, R_STUDIODRAWPOINTS);
	localPlayer = localPlayer.scan(CLIENT_DLL, LOCAL_PLAYER, 0x2).deRef();
	csgoHud = csgoHud.scan(CLIENT_DLL, CSGO_HUD, 0x1).deRef();
	hudfindElement = hudfindElement.scan(CLIENT_DLL, FIND_ELEMENT);
	keyValuesFromString = keyValuesFromString.scan(CLIENT_DLL, KEY_VALUES_FROM_STR);
	animOverlays = animOverlays.scan(CLIENT_DLL, ANIMATION_LAYER, 0x2).deRef();
	sequenceActivity = sequenceActivity.scan(CLIENT_DLL, SEQUENCE_ACTIVITY);
	cachedBones = cachedBones.scan(CLIENT_DLL, CACHED_BONE, 0x2).deRef().add(0x4);
	setAbsOrigin = setAbsOrigin.scan(CLIENT_DLL, SETABSORIGIN);
	isC4Owner = isC4Owner.scan(CLIENT_DLL, HASC4);
	isBreakable = isBreakable.scan(CLIENT_DLL, IS_BREAKBLE);
	predictionData = predictionData.scan(CLIENT_DLL, PREDICTION_MOVE_DATA, 0x1).deRef(Dereference::TWICE);
	predictionSeed = predictionSeed.scan(CLIENT_DLL, PREDICTIONRANDOMSEED, 0x2).deRef();
	flashlightCreate = flashlightCreate.scan(CLIENT_DLL, FLASHLIGHT_CREATE);
	flashlightUpdate = flashlightUpdate.scan(CLIENT_DLL, FLASHLIGHT_UPDATE);
	flashlightDestroy = flashlightDestroy.scan(CLIENT_DLL, FLASHLIGHT_DESTROY);
	occlusion = occlusion.scan(CLIENT_DLL, SETUP_OCCLUSION);
	velocity = velocity.scan(CLIENT_DLL, SETUP_VELOCITY);
	accumulate = accumulate.scan(CLIENT_DLL, SETUP_VELOCITY);
	particleIsCached = particleIsCached.scan(CLIENT_DLL, IS_EFFECT_CACHED);
	particleSystem = particleSystem.scan(CLIENT_DLL, PARTICLE_SYSTEM, 0x7).deRef(Dereference::TWICE);
	particleFindStringIndex = particleFindStringIndex.scan(CLIENT_DLL, FIND_STRING_INDEX);
	particleCall = particleCall.scan(CLIENT_DLL, PARTICLE_CALL);
	particleSetControlPoint = particleSetControlPoint.scan(CLIENT_DLL, SET_PARTICLE_POINT);
	predictedPlayer = predictedPlayer.scan(CLIENT_DLL, PREDICTED_PLAYER).add(0x2).deRef();
	physicsRunThink = physicsRunThink.scan(CLIENT_DLL, PHYSICS_RUN_THINK);
	lastCommand = lastCommand.scan(CLIENT_DLL, LAST_COMMAND).add(0x2).deRef();
	retAddrToInterpolation = retAddrToInterpolation.scan(CLIENT_DLL, RET_ADDR_INTERPOLATION);
	postThinkPhysics = postThinkPhysics.scan(CLIENT_DLL, POST_THINK_PHYSICS);
	simulateEntities = simulateEntities.scan(CLIENT_DLL, SIMULATE_ENTITIES);
	vecClientImpacts = vecClientImpacts.scan(CLIENT_DLL, VEC_CLIENT_IPACT_LIST).add(0x2).deRef();
	loadFromBuffer = loadFromBuffer.scan(CLIENT_DLL, LOAD_FROM_BUFFER);
	keyValuesConstruct = keyValuesConstruct.scan(CLIENT_DLL, KEYVALUES_CONSTRUCT);
	keyValuesDestruct = keyValuesDestruct.scan(CLIENT_DLL, KEYVALUES_DESTRUCT);
	checkThinkFunction = checkThinkFunction.scan(CLIENT_DLL, CHECK_HAS_THINK_FN);
	usingStandardWeaponsVehicle = usingStandardWeaponsVehicle.scan(CLIENT_DLL, USING_STANDARD_WEAPONS_VEH);
	selectItem = selectItem.scan(CLIENT_DLL, SELECT_ITEM);
	transferData = transferData.scan(CLIENT_DLL, TRANSFER_DATA);
	reinitPredicatbles = reinitPredicatbles.scan(CLIENT_DLL, REINIT_PREDICTABLES);
	shutdownPredicatbles = shutdownPredicatbles.scan(CLIENT_DLL, SHUTDOWN_PREDICTABLES);

	// HOOKS

	clientValidAddr = clientValidAddr.scan(CLIENT_DLL, NEW_CHECK);
	enginevalidAddr = enginevalidAddr.scan(ENGINE_DLL, NEW_CHECK);
	studioRenderValidAddr = studioRenderValidAddr.scan(STUDIORENDER_DLL, NEW_CHECK);
	materialSysValidAddr = materialSysValidAddr.scan(MATERIAL_DLL, NEW_CHECK);
	isUsingPropDebug = isUsingPropDebug.scan(ENGINE_DLL, IS_USING_PROP_DEBUG).rel(0x1);
	getColorModulation = getColorModulation.scan(MATERIAL_DLL, GET_COLOR_MODULATION);
	extraBonesProcessing = extraBonesProcessing.scan(CLIENT_DLL, EXTRA_BONES_PROCCESSING);
	buildTransformations = buildTransformations.scan(CLIENT_DLL, BUILD_TRANSFORMATIONS);
	particleSimulate = particleSimulate.scan(CLIENT_DLL, PARTICLE_SIMULATE);
	sendDataGram = sendDataGram.scan(ENGINE_DLL, SEND_DATAGRAM);
	unkOverviewMap = unkOverviewMap.scan(CLIENT_DLL, UNK_OVERVIEWMAP);
	isDepth = isDepth.scan(CLIENT_DLL, IS_DEPTH);
	fxBlood = fxBlood.scan(CLIENT_DLL, FX_BLOOD);
	addEnt = addEnt.scan(CLIENT_DLL, ADD_ENT);
	removeEnt = removeEnt.scan(CLIENT_DLL, REMOVE_ENT);
	isFollowedEntity = isFollowedEntity.scan(CLIENT_DLL, IS_FOLLOWED_ENT);
	spottedEntityUpdate = spottedEntityUpdate.scan(CLIENT_DLL, SPOTTED_ENTITIY_UPDATE);
	fireInternfn = fireInternfn.scan(ENGINE_DLL, FIRE_INTERN);
	preRound = preRound.scan(SERVER_DLL, SERVER_PREROUND);
	playSoundStep = playSoundStep.scan(CLIENT_DLL, PLAY_STEP_SOUND);

	// REST

	tesla = tesla.scan(CLIENT_DLL, FX_TESLA);
	dispatchEffect = dispatchEffect.scan(CLIENT_DLL, DISPATCH_EFFECT);

	// SDK INTERFACES

	game::localPlayer.init();
	memory::interfaces::init();

	beams = beams.scan(CLIENT_DLL, BEAMS, 0x1).deRef();
	glowManager = glowManager.scan(CLIENT_DLL, GLOWMANAGER, 0x3).deRef();
	weaponInterface = weaponInterface.scan(CLIENT_DLL, WEAPONDATA, 0x2).deRef();
	moveHelper = moveHelper.scan(CLIENT_DLL, MOVEHELPER, 0x2).deRef(Dereference::TWICE);
	dx9Device = dx9Device.scan(SHARED_API, DX9_DEVICE, 0x1).deRef(Dereference::TWICE);
	clientState = clientState.scan(ENGINE_DLL, CLIENT_STATE, 0x1).deRef(Dereference::TWICE);
	viewRender = viewRender.scan(CLIENT_DLL, VIEW_RENDER, 0x2).deRef(Dereference::TWICE);

	keyValuesSys = keyValuesSys.byExport<"KeyValuesSystem"_hasher>(VSTD_DLL).cast<keyValuesSystem_t>()()();
	memAlloc = memAlloc.byExport<"g_pMemAlloc"_hasher>(TIER_DLL).deRef();
	moveHelper = moveHelper.scan(CLIENT_DLL, MOVEHELPER, 0x2).deRef(Dereference::TWICE);
	beams = beams.scan(CLIENT_DLL, BEAMS, 0x1).deRef();
	
	globalVars = globalVars.byVFunc(memory::interfaces::client, 0).add(0x1F).deRef(Dereference::TWICE);
	clientMode = clientMode.byVFunc(memory::interfaces::client, 10).add(0x5).deRef(Dereference::TWICE);
	input = input.byVFunc(memory::interfaces::client, 16).add(0x1).deRef();

	postInit();

	console::info("memory init success");
}

void memory::postInit()
{
	using namespace memory::interfaces;

	preciptation = preciptation.findFromGame(CPrecipitation);
	resourceInterface = resourceInterface.findFromGameLoop(CCSPlayerResource);
	gameRules = gameRules.findFromGameLoop(CCSGameRulesProxy);
}