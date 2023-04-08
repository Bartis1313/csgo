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
template<size_t N>
memory::Address<T> memory::Address<T>::scan(const std::string_view mod, const std::array<std::optional<uint8_t>, N>& sig)
{
	const auto _module = memory::getModule(mod);
	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(_module);
	const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uint8_t*>(_module) + dosHeader->e_lfanew);
	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;

	auto check = [sig](uint8_t* data)
	{
		for (const auto& _byte : sig)
		{
			if (_byte && _byte.value() != *data)
				return false;
			data++;
		}
		return true;
	};

	for (auto i : std::views::iota(reinterpret_cast<size_t>(_module), static_cast<size_t>(sizeOfImage + reinterpret_cast<size_t>(_module))))
	{
		if (check(reinterpret_cast<uint8_t*>(i)))
		{
			m_addr = i;
			break;
		}
	}

	if (!m_addr)	
		throw std::runtime_error{ std::format("Couldn't find signature, module {}, signature: {}", mod, signature::to_string(sig)) };

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
#include <SDK/IClientEntityList.hpp>

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

#include <SDK/IViewRenderBeams.hpp>

void memory::init()
{
#define ADD_TO_MAP(name) \
	detail::m_ModulesAddr[name] = reinterpret_cast<HMODULE>(LI_MODULE(name).cached()); \

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

	traceFilterSimple = traceFilterSimple.scan(CLIENT_DLL, CTRACE_FILTER_SIMPLE).add(0x3D);
	returnAddrRadarImage = returnAddrRadarImage.scan(PANORAMA_DLL, UNK_FILESYS);
	viewMatrixAddr = viewMatrixAddr.scan(CLIENT_DLL, VIEW_MATRIX_CLIENT).add(0x3).deRef().add(0xB0);
	drawScreenEffectMaterial = drawScreenEffectMaterial.scan(CLIENT_DLL, DRAW_EFFECT_MATRIAL);
	motionBlurVec = motionBlurVec.scan(CLIENT_DLL, BLUR_MATERIAL_ARR_1).add(0x1).deRef();
	disableTargetAlloc = disableTargetAlloc.scan(MATERIAL_DLL, DISABLE_RENDER_TARGET_ALLOC).add(0x2);
	throughSmoke = throughSmoke.scan(CLIENT_DLL, GOES_THROUGH_SMOKE);
	smokeCount = throughSmoke.add(0x8).deRef().cast<uintptr_t>();
	loadSky = loadSky.scan(ENGINE_DLL, LOAD_SKY);
	callbacksHead = callbacksHead.scan(CLIENT_DLL, HEAD_OF_EFFECTS).add(0x2).deRef(Dereference::TWICE);
	camThink = camThink.scan(CLIENT_DLL, CAM_THINK);
	renderDrawPoints = renderDrawPoints.scan(STUDIORENDER_DLL, R_STUDIODRAWPOINTS);
	localPlayer = localPlayer.scan(CLIENT_DLL, LOCAL_PLAYER).add(0x2).deRef();
	csgoHud = csgoHud.scan(CLIENT_DLL, CSGO_HUD).add(0x1).deRef();
	hudfindElement = hudfindElement.scan(CLIENT_DLL, FIND_ELEMENT);
	keyValuesFromString = keyValuesFromString.scan(CLIENT_DLL, KEY_VALUES_FROM_STR);
	animOverlays = animOverlays.scan(CLIENT_DLL, ANIMATION_LAYER).add(0x2).deRef();
	sequenceActivity = sequenceActivity.scan(CLIENT_DLL, SEQUENCE_ACTIVITY);
	cachedBones = cachedBones.scan(CLIENT_DLL, CACHED_BONE).add(0x2).deRef().add(0x4);
	setAbsOrigin = setAbsOrigin.scan(CLIENT_DLL, SETABSORIGIN);
	isC4Owner = isC4Owner.scan(CLIENT_DLL, HASC4);
	isBreakable = isBreakable.scan(CLIENT_DLL, IS_BREAKBLE);
	predictionData = predictionData.scan(CLIENT_DLL, PREDICTION_MOVE_DATA).add(0x1).deRef(Dereference::TWICE);
	predictionSeed = predictionSeed.scan(CLIENT_DLL, PREDICTIONRANDOMSEED).add(0x2).deRef();
	flashlightCreate = flashlightCreate.scan(CLIENT_DLL, FLASHLIGHT_CREATE);
	flashlightUpdate = flashlightUpdate.scan(CLIENT_DLL, FLASHLIGHT_UPDATE);
	flashlightDestroy = flashlightDestroy.scan(CLIENT_DLL, FLASHLIGHT_DESTROY);
	occlusion = occlusion.scan(CLIENT_DLL, SETUP_OCCLUSION);
	velocity = velocity.scan(CLIENT_DLL, SETUP_VELOCITY);
	accumulate = accumulate.scan(CLIENT_DLL, SETUP_VELOCITY);
	particleIsCached = particleIsCached.scan(CLIENT_DLL, IS_EFFECT_CACHED);
	particleSystem = particleSystem.scan(CLIENT_DLL, PARTICLE_SYSTEM).add(0x7).deRef(Dereference::TWICE);
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
	destroyMaterial = destroyMaterial.scan(MATERIAL_DLL, MATERIAL_DESTRUCT);
	allocKeyValuesClient = allocKeyValuesClient.scan(CLIENT_DLL, RET_ADDR_KEYVALUES_CLIENT).add(0x3E);
	allocKeyValuesEngine = allocKeyValuesEngine.scan(ENGINE_DLL, RET_ADDR_KEYVALUES_ENGINE).add(0x4A);
	flashbangRet = flashbangRet.scan(CLIENT_DLL, FLASH_EFFECT_RET).add(0x3);
	flashbangWhiteRet = flashbangWhiteRet.scan(CLIENT_DLL, FLASH_EFFECT_WHITE_RET).add(0xA);
	precipitationClientThink = precipitationClientThink.scan(CLIENT_DLL, CLIENT_THINK_PRECIPITATION);
	precipitationInit = precipitationInit.scan(CLIENT_DLL, PRECIPITATION_INIT);
	takeDmg = takeDmg.scan(CLIENT_DLL, TAKE_DMG).add(0x2).deRef();
	setAbsVelocity = setAbsVelocity.scan(CLIENT_DLL, SET_ABS_VELOCITY);

	// HOOKS

	isUsingPropDebug = isUsingPropDebug.scan(ENGINE_DLL, IS_USING_PROP_DEBUG).rel(0x1);
	getColorModulation = getColorModulation.scan(MATERIAL_DLL, GET_COLOR_MODULATION);
	extraBonesProcessing = extraBonesProcessing.scan(CLIENT_DLL, EXTRA_BONES_PROCCESSING);
	buildTransformations = buildTransformations.scan(CLIENT_DLL, BUILD_TRANSFORMATIONS);
	particleSimulate = particleSimulate.scan(CLIENT_DLL, PARTICLE_SIMULATE);
	sendDataGram = sendDataGram.scan(ENGINE_DLL, SEND_DATAGRAM);
	unkOverviewMap = unkOverviewMap.scan(CLIENT_DLL, UNK_OVERVIEWMAP);
	isDepth = isDepth.scan(CLIENT_DLL, IS_DEPTH);
	fxBlood = fxBlood.scan(CLIENT_DLL, FX_BLOOD);
	fxBloodSpray = fxBloodSpray.scan(CLIENT_DLL, FX_BLOOD_SPRAY);
	bloodCallback = bloodCallback.scan(CLIENT_DLL, BLOOD_CALLBACK);
	addEnt = addEnt.scan(CLIENT_DLL, ADD_ENT);
	removeEnt = removeEnt.scan(CLIENT_DLL, REMOVE_ENT);
	isFollowedEntity = isFollowedEntity.scan(CLIENT_DLL, IS_FOLLOWED_ENT);
	spottedEntityUpdate = spottedEntityUpdate.scan(CLIENT_DLL, SPOTTED_ENTITIY_UPDATE);
	fireInternfn = fireInternfn.scan(ENGINE_DLL, FIRE_INTERN);
	preRound = preRound.scan(CLIENT_DLL, GAME_INSTRUCTOR_EVENT);
	playSoundStep = playSoundStep.scan(CLIENT_DLL, PLAY_STEP_SOUND);
	restoreData = restoreData.scan(CLIENT_DLL, RESTORE_DATA);
	saveData = saveData.scan(CLIENT_DLL, SAVE_DATA);
	bloodUtilCallback = bloodUtilCallback.scan(CLIENT_DLL, BLOOD_UTIL_CALLBACK);
	chudIsHidden = chudIsHidden.scan(CLIENT_DLL, CHUD_IS_HIDDEN);
	viewFade = viewFade.scan(ENGINE_DLL, VIEW_FADE);
	unkRound = unkRound.scan(CLIENT_DLL, UNK_SURVIVAL_RELATED);

	// REST

	tesla = tesla.scan(CLIENT_DLL, FX_TESLA);
	dispatchEffect = dispatchEffect.scan(CLIENT_DLL, DISPATCH_EFFECT);

	// SDK INTERFACES

	game::localPlayer.init();
	memory::interfaces::init();

	glowManager = glowManager.scan(CLIENT_DLL, GLOWMANAGER).add(0x3).deRef();
	weaponInterface = weaponInterface.scan(CLIENT_DLL, WEAPONDATA).add(0x2).deRef();
	moveHelper = moveHelper.scan(CLIENT_DLL, MOVEHELPER).add(0x2).deRef(Dereference::TWICE);
	dx9Device = dx9Device.scan(SHARED_API, DX9_DEVICE).add(0x1).deRef(Dereference::TWICE);
	clientState = clientState.scan(ENGINE_DLL, CLIENT_STATE).add(0x1).deRef(Dereference::TWICE);
	viewRender = viewRender.scan(CLIENT_DLL, VIEW_RENDER).add(0x2).deRef(Dereference::TWICE);

	keyValuesSys = keyValuesSys.byExport<"KeyValuesSystem"_hasher>(VSTD_DLL).cast<keyValuesSystem_t>()()();
	memAlloc = memAlloc.byExport<"g_pMemAlloc"_hasher>(TIER_DLL).deRef();
	moveHelper = moveHelper.scan(CLIENT_DLL, MOVEHELPER).add(0x2).deRef(Dereference::TWICE);
	beams = beams.scan(CLIENT_DLL, BEAMS).add(0x1).deRef();
	
	globalVars = globalVars.byVFunc(memory::interfaces::client, 0).add(0x1F).deRef(Dereference::TWICE);
	clientMode = clientMode.byVFunc(memory::interfaces::client, 10).add(0x5).deRef(Dereference::TWICE);
	input = input.byVFunc(memory::interfaces::client, 16).add(0x1).deRef();

	postInit();

	console::debug("memory init success");
}

void memory::postInit()
{
	using namespace memory::interfaces;

	preciptation = preciptation.findFromGame(CPrecipitation);
	resourceInterface = resourceInterface.findFromGameLoop(CCSPlayerResource);
	gameRules = gameRules.findFromGameLoop(CCSGameRulesProxy);
}