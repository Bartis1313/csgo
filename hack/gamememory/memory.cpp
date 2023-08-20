#include "memory.hpp"

#include "sigs.hpp"
#include "modules.hpp"

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
#include <SDK/IViewRenderBeams.hpp>

void memory::init()
{
	memory::modules::addModule<"engine.dll">();
	memory::modules::addModule<"client.dll">();
	memory::modules::addModule<"vstdlib.dll">();
	memory::modules::addModule<"vgui2.dll">();
	memory::modules::addModule<"vguimatsurface.dll">();
	memory::modules::addModule<"materialsystem.dll">();
	memory::modules::addModule<"localize.dll">();
	memory::modules::addModule<"studiorender.dll">();
	memory::modules::addModule<"inputsystem.dll">();
	memory::modules::addModule<"shaderapidx9.dll">();
	memory::modules::addModule<"tier0.dll">();
	memory::modules::addModule<"panorama.dll">();
	memory::modules::addModule<"filesystem_stdio.dll">();
	memory::modules::addModule<"datacache.dll">();
	memory::modules::addModule<"gameoverlayrenderer.dll">();
	memory::modules::addModule<"vphysics.dll">();

	memory::interfaces::init();

	using namespace memory;
	using namespace memory::interfaces;

	traceFilterSimple = scan(CLIENT_DLL, CTRACE_FILTER_SIMPLE).add(0x3D);
	returnAddrRadarImage = scan(PANORAMA_DLL, UNK_FILESYS);
	viewMatrixAddr = scan(CLIENT_DLL, VIEW_MATRIX_CLIENT).add(0x3).deRef().add(0xB0);
	drawScreenEffectMaterial = scan(CLIENT_DLL, DRAW_EFFECT_MATRIAL);
	motionBlurVec = scan(CLIENT_DLL, BLUR_MATERIAL_ARR_1).add(0x1).deRef();
	throughSmoke = scan(CLIENT_DLL, GOES_THROUGH_SMOKE);
	smokeCount = throughSmoke.add(0x8).deRef().cast<uintptr_t>();
	loadSky = scan(ENGINE_DLL, LOAD_SKY);
	callbacksHead = scan(CLIENT_DLL, HEAD_OF_EFFECTS).add(0x2).deRef(Dereference::TWICE);
	camThink = scan(CLIENT_DLL, CAM_THINK);
	renderDrawPoints = scan(STUDIORENDER_DLL, R_STUDIODRAWPOINTS);
	localPlayer = scan(CLIENT_DLL, LOCAL_PLAYER).add(0x2).deRef(); game::localPlayer.init();
	csgoHud = scan(CLIENT_DLL, CSGO_HUD).add(0x1).deRef();
	hudfindElement = scan(CLIENT_DLL, FIND_ELEMENT);
	keyValuesFromString = scan(CLIENT_DLL, KEY_VALUES_FROM_STR);
	animOverlays = scan(CLIENT_DLL, ANIMATION_LAYER).add(0x2).deRef();
	sequenceActivity = scan(CLIENT_DLL, SEQUENCE_ACTIVITY);
	cachedBones = scan(CLIENT_DLL, CACHED_BONE).add(0x2).deRef().add(0x4);
	setAbsOrigin = scan(CLIENT_DLL, SETABSORIGIN);
	setAbsAngle = scan(CLIENT_DLL, SETABSANGLES);
	isC4Owner = scan(CLIENT_DLL, HASC4);
	isBreakable = scan(CLIENT_DLL, IS_BREAKBLE);
	predictionData = scan(CLIENT_DLL, PREDICTION_MOVE_DATA).add(0x1).deRef(Dereference::TWICE);
	predictionSeed = scan(CLIENT_DLL, PREDICTIONRANDOMSEED).add(0x2).deRef();
	flashlightCreate = scan(CLIENT_DLL, FLASHLIGHT_CREATE);
	flashlightUpdate = scan(CLIENT_DLL, FLASHLIGHT_UPDATE);
	flashlightDestroy = scan(CLIENT_DLL, FLASHLIGHT_DESTROY);
	occlusion = scan(CLIENT_DLL, SETUP_OCCLUSION);
	velocity = scan(CLIENT_DLL, SETUP_VELOCITY);
	accumulate = scan(CLIENT_DLL, ACCUMULATE_LAYERS);
	particleIsCached = scan(CLIENT_DLL, IS_EFFECT_CACHED);
	particleSystem = scan(CLIENT_DLL, PARTICLE_SYSTEM).add(0x7).deRef(Dereference::TWICE);
	particleFindStringIndex = scan(CLIENT_DLL, FIND_STRING_INDEX);
	particleCall = scan(CLIENT_DLL, PARTICLE_CALL);
	particleSetControlPoint = scan(CLIENT_DLL, SET_PARTICLE_POINT);
	predictedPlayer = scan(CLIENT_DLL, PREDICTED_PLAYER).add(0x2).deRef();
	physicsRunThink = scan(CLIENT_DLL, PHYSICS_RUN_THINK);
	lastCommand = scan(CLIENT_DLL, LAST_COMMAND).add(0x2).deRef();
	retAddrToInterpolation = scan(CLIENT_DLL, RET_ADDR_INTERPOLATION);
	postThinkPhysics = scan(CLIENT_DLL, POST_THINK_PHYSICS);
	simulateEntities = scan(CLIENT_DLL, SIMULATE_ENTITIES);
	vecClientImpacts = scan(CLIENT_DLL, VEC_CLIENT_IPACT_LIST).add(0x2).deRef();
	loadFromBuffer = scan(CLIENT_DLL, LOAD_FROM_BUFFER);
	keyValuesConstruct = scan(CLIENT_DLL, KEYVALUES_CONSTRUCT);
	keyValuesDestruct = scan(CLIENT_DLL, KEYVALUES_DESTRUCT);
	checkThinkFunction = scan(CLIENT_DLL, CHECK_HAS_THINK_FN);
	usingStandardWeaponsVehicle = scan(CLIENT_DLL, USING_STANDARD_WEAPONS_VEH);
	selectItem = scan(CLIENT_DLL, SELECT_ITEM);
	transferData = scan(CLIENT_DLL, TRANSFER_DATA);
	reinitPredicatbles = scan(CLIENT_DLL, REINIT_PREDICTABLES);
	shutdownPredicatbles = scan(CLIENT_DLL, SHUTDOWN_PREDICTABLES);
	destroyMaterial = scan(MATERIAL_DLL, MATERIAL_DESTRUCT);
	allocKeyValuesClient = scan(CLIENT_DLL, RET_ADDR_KEYVALUES_CLIENT).add(0x3E);
	allocKeyValuesEngine = scan(ENGINE_DLL, RET_ADDR_KEYVALUES_ENGINE).add(0x4A);
	flashbangRet = scan(CLIENT_DLL, FLASH_EFFECT_RET).add(0x3);
	flashbangWhiteRet = scan(CLIENT_DLL, FLASH_EFFECT_WHITE_RET).add(0xA);
	precipitationClientThink = scan(CLIENT_DLL, CLIENT_THINK_PRECIPITATION);
	precipitationInit = scan(CLIENT_DLL, PRECIPITATION_INIT);
	takeDmg = scan(CLIENT_DLL, TAKE_DMG).add(0x2).deRef();
	setAbsVelocity = scan(CLIENT_DLL, SET_ABS_VELOCITY);
	firstMoveChild = scan(CLIENT_DLL, FIRST_MOVE_CHILD).add(0x2).deRef();
	nextMovePeer = scan(CLIENT_DLL, NEXT_PEER).add(0x2).deRef();
	getParticleSystemIndex = scan(CLIENT_DLL, GET_PARTICLE_SYSTEM_IDX);
	dispatchParticleEffect = scan(CLIENT_DLL, DISPATCH_PARTICLE_EFFECT);
	renderBoxInternal = scan(CLIENT_DLL, RENDER_BOX);
	renderLine = scan(CLIENT_DLL, RENDER_LINE);
	vignetteBlurStrengthPost = scan(CLIENT_DLL, BLURSCOPE_VIGNETTEPOST).add(0x3).deRef().add(0x4);
	scopeDust = scan(CLIENT_DLL, SCOPE_DUST_RET).add(0x3);
	scopeArc = scan(CLIENT_DLL, SCOPE_ARC_RET);
	addGlowBox = scan(CLIENT_DLL, ADD_GLOW_BOX);
	setString = scan(ENGINE_DLL, KEY_VALUES_SET_STRING);
	findKey = scan(ENGINE_DLL, KEY_VALUES_FIND_KEY);
	decalPool = scan(ENGINE_DLL, S_ADECALPOOL).add(0x1).deRef();
	valveHook = scan(GAME_OVERLAY, VALVE_HOOK);
	valveUnHook = scan(GAME_OVERLAY, VALVE_UNHOOK);
	viewFadeSmokeRet = scan(CLIENT_DLL, VIEW_FACE_SMOKE_RET).add(0xC);
	gPrecipitations = scan(CLIENT_DLL, G_PRECIPICATION).add(0x1).deRef();
	precipDestruct = scan(CLIENT_DLL, DESTRUCT_PRECIPICATION);
	stopSound = scan(CLIENT_DLL, STOP_SOUND);
	particleGetVCollideRet = scan(CLIENT_DLL, WEATHER_GETVCOLLIDE_RET).add(0x2D);
	vPhysicsGetObject = scan(CLIENT_DLL, VPHYSICS_GET_OBJ).add(0x2).deRef();
	restoreMaterialSystemObjects = scan(ENGINE_DLL, RESTOTRE_MATERIALS);
	forceSingleThreaded = scan(MATERIAL_DLL, FORCE_SINGLE_THREADED);
	precacheVars = scan(MATERIAL_DLL, PRECACHE_VARS);
	levelName = scan(ENGINE_DLL, LEVEL_NAME).add(0x2).deRef();

	// HOOKS

	isUsingPropDebug = scan(ENGINE_DLL, IS_USING_PROP_DEBUG).rel(0x1);
	getColorModulation = scan(MATERIAL_DLL, GET_COLOR_MODULATION);
	extraBonesProcessing = scan(CLIENT_DLL, EXTRA_BONES_PROCCESSING);
	buildTransformations = scan(CLIENT_DLL, BUILD_TRANSFORMATIONS);
	particleSimulate = scan(CLIENT_DLL, PARTICLE_SIMULATE);
	sendDataGram = scan(ENGINE_DLL, SEND_DATAGRAM);
	unkOverviewMap = scan(CLIENT_DLL, UNK_OVERVIEWMAP);
	isDepth = scan(CLIENT_DLL, IS_DEPTH);
	fxBlood = scan(CLIENT_DLL, FX_BLOOD);
	fxBloodSpray = scan(CLIENT_DLL, FX_BLOOD_SPRAY);
	bloodCallback = scan(CLIENT_DLL, BLOOD_CALLBACK);
	addEnt = scan(CLIENT_DLL, ADD_ENT);
	removeEnt = scan(CLIENT_DLL, REMOVE_ENT);
	isFollowedEntity = scan(CLIENT_DLL, IS_FOLLOWED_ENT);
	spottedEntityUpdate = scan(CLIENT_DLL, SPOTTED_ENTITIY_UPDATE);
	fireInternfn = scan(ENGINE_DLL, FIRE_INTERN);
	preRound = scan(CLIENT_DLL, GAME_INSTRUCTOR_EVENT);
	playSoundStep = scan(CLIENT_DLL, PLAY_STEP_SOUND);
	restoreData = scan(CLIENT_DLL, RESTORE_DATA);
	saveData = scan(CLIENT_DLL, SAVE_DATA);
	bloodUtilCallback = scan(CLIENT_DLL, BLOOD_UTIL_CALLBACK);
	chudIsHidden = scan(CLIENT_DLL, CHUD_IS_HIDDEN);
	viewFade = scan(ENGINE_DLL, VIEW_FADE);
	unkRound = scan(CLIENT_DLL, UNK_SURVIVAL_RELATED);
	present = scan(GAME_OVERLAY, DX9_PRESENT).add(0x2);
	reset = scan(GAME_OVERLAY, DX9_RESET).add(0x9);
	updatePostEffects = scan(CLIENT_DLL, UPDATEPOST_EFFECTS);
	getPMaterial = scan(CLIENT_DLL, GET_PMATERIAL);
	decalAddToSurface = scan(ENGINE_DLL, R_DECAL_ADD_TO_SURFACE);
	createDecal = scan(ENGINE_DLL, R_DECAL_CREATE);
	createParticlePrecip = scan(CLIENT_DLL, CREATE_PARTICLE_PRECIP);
	initializeParticlePrecip = scan(CLIENT_DLL, INITIALIZE_PRECIP_PARTICLE);
	viewDrawScene = scan(CLIENT_DLL, VIEW_DRAW_SCENE);
	drawEffects = scan(CLIENT_DLL, DRAW_EFFECTS);
	drawWorldAndEntities = scan(CLIENT_DLL, DRAW_WORLD_AND_ENTITIES);
	drawTransculentRenderables = scan(CLIENT_DLL, DRAW_TRANSCULENT_RENDERABLE);
	clientCsNormalEvent = scan(CLIENT_DLL, CLIENTMODE_CSNORMAL_EVENT);
	tracerDraw = scan(CLIENT_DLL, TRACER_DRAW);
	dispatchInnerParticlePrecip = scan(CLIENT_DLL, DISPATCH_INNER_PRECIP);
	newParticleCreate = scan(CLIENT_DLL, NEW_PARTICLE_EFFECT_CREATE);
	newParticleSetControlPoint = scan(CLIENT_DLL, NEW_PARTICLE_SET_POINT);
	newParticleSetControlPointEnt = scan(CLIENT_DLL, NEW_PARTICLE_SET_POINT_ENT);
	getVelocity = scan(CLIENT_DLL, GET_VELOCITY).add(0x2);

	// REST

	tesla = scan(CLIENT_DLL, FX_TESLA);
	dispatchEffect = scan(CLIENT_DLL, DISPATCH_EFFECT);

	// SDK INTERFACES

	glowManager = scan(CLIENT_DLL, GLOWMANAGER).add(0x3).deRef();
	weaponInterface = scan(CLIENT_DLL, WEAPONDATA).add(0x2).deRef();
	moveHelper = scan(CLIENT_DLL, MOVEHELPER).add(0x2).deRef(Dereference::TWICE);
	dx9Device = scan(SHARED_API, DX9_DEVICE).add(0x1).deRef(Dereference::TWICE);
	clientState = scan(ENGINE_DLL, CLIENT_STATE).add(0x1).deRef(Dereference::TWICE);
	viewRender = scan(CLIENT_DLL, VIEW_RENDER).add(0x2).deRef(Dereference::TWICE);

	keyValuesSys = byExport<"KeyValuesSystem"_hasher>(VSTD_DLL).cast<keyValuesSystem_t>()()();
	memAlloc = byExport<"g_pMemAlloc"_hasher>(TIER_DLL).deRef();

	moveHelper = scan(CLIENT_DLL, MOVEHELPER).add(0x2).deRef(Dereference::TWICE);
	beams = scan(CLIENT_DLL, BEAMS).add(0x1).deRef();

	globalVars = byVFunc(memory::interfaces::client, 0).add(0x1F).deRef(Dereference::TWICE);
	clientMode = byVFunc(memory::interfaces::client, 10).add(0x5).deRef(Dereference::TWICE);
	input = byVFunc(memory::interfaces::client, 16).add(0x1).deRef();

	postInit();
	netvars::init();

	console::debug("memory init success");
}


void memory::postInit()
{
	using namespace memory::interfaces;

	preciptation = findFromGame(CPrecipitation);
	resourceInterface = findFromGameLoop(CCSPlayerResource);
	gameRules = findFromGameLoop(CCSGameRulesProxy);
	toneController = findFromGameLoop(CEnvTonemapController);
	fogController = findFromGameLoop(CFogController);
	ambientLight = findFromGameLoop(CEnvAmbientLight);
	dynamicLight = findFromGame(CDynamicLight);

	hostState = scan(ENGINE_DLL, HOST_STATE).add(0x1).deRef(Dereference::TWICE);
	gameWorld = scan(CLIENT_DLL, CLIENT_WORLD).add(0x1).deRef(Dereference::TWICE);

	console::debug("memory post init success");
}