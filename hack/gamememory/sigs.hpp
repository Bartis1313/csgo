#pragma once

// I found by strictly setting glow color in console and finding what exactly changed in reclass
// or go EntityGlowEffects, list up xrefs. https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/glow_outline_effect.cpp#L34
#define GLOWMANAGER                     "0F 11 05 ? ? ? ? 83 C8 01"
// const CViewRenderBeams::`vftable should be pushed in:
// #STR #STR: "r_drawbrushmodels", "engine/writez", "debug/debugtranslucentsinglecolor", "engine/modulatesinglecolor", "vgui/white", "r_drawentities", "Other textures"
#define BEAMS                           "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9"
// #STR: "%s stuck on object %i/%s", "client"
#define MOVEHELPER                      "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01"
// #STR SelectWeightedSequence
// or never sig it and use exactly code that game uses
#define PREDICTIONRANDOMSEED            "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04"
// no STR
// it will be easier to find by live debugger
// or just find it below setabs one which is easier to find
#define SETABSANGLES                    "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"
// Ignoring unreasonable position (%f,%f,%"... is a hint
#define SETABSORIGIN                    "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"
// should call check ammo cvar or smth
// #STR: ammo_50AE_impulse
#define WEAPONDATA                      "8B 35 ? ? ? ? FF 10 0F B7 C0"
// #STR: "weapon_c4"
#define HASC4                           "56 8B F1 85 F6 74 31"
// #STR: (mod_studio)
#define INVALIDATE_BONE_CACHE           "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81"
// useless since in here you can read it can be forced, but it's safer
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/213556-changing-sky-spoofing-cvar.html
#define LOAD_SKY                        "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"
// anything with literal named bones/hitboxes
#define ANIMATION_LAYER                 "8B 89 ? ? ? ? 8D 0C D1"
// #STR: "No model!", "Not Found!
// and anything with sequence
#define SEQUENCE_ACTIVITY               "55 8B EC 53 8B 5D 08 56 8B F1 83"
// also: 55 8B EC 8B 45 10 50 8B 4D 04
// #STR: "value2", "value"
#define RET_ADDR_KEYVALUES_ENGINE       "55 8B EC 56 57 8B F9 8B F2 83 FF 11 0F 87"
// #STR: "value2", "value"
#define RET_ADDR_KEYVALUES_CLIENT       "55 8B EC 56 57 8B F9 8B F2 83 FF 11 0F 87"
// #STR: "Video driver has crashed and been reset, re-uploading reso
#define DX9_DEVICE                      "A1 ? ? ? ? 50 8B 08 FF 51 0C"
// https://aixxe.net/2017/09/steam-overlay-rendering
#define DX9_PRESENT                     "FF 15 ? ? ? ? 8B F0 85 FF"
// https://aixxe.net/2017/09/steam-overlay-rendering
// less safe sig: FF 15 ? ? ? ? 8B D8 85 DB 78 18
#define DX9_RESET                       "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8"
// #STR: DebugView
// #STR: debug/
// finding it with live debugger is easier and I don't think it needs any explanation how to find your local view matrix
#define VIEW_MATRIX_CLIENT              "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"
// #STR: "func_breakable", "func_breakable_surf"
#define IS_BREAKBLE                     "55 8B EC 51 56 8B F1 85 F6 74 68"
// this is needed because this convar will rely on many things, the r_drawspecificstaticprop and forcing isn't enough
// #STR: "CStaticProp::DrawModel", "Static_Prop_Rendering"
// just like in engine source, before Static_Prop_Rendering there will be a check before to that function
// first call is that function
// now, normal sig from selection points to many other functions so use a relative one
#define IS_USING_PROP_DEBUG             "E8 ? ? ? ? 84 C0 8B 45 08"
// see how many pushes are logically to color struct
// should call this one:
// #STR: "CMaterial::PrecacheVars: error loading vmt file for %s\n", "vmt_patches"
#define GET_COLOR_MODULATION            "55 8B EC 83 EC ? 56 8B F1 8A 46"
// disable useless processing on setupbones
// #STR: "ankle_R", "hand_L", "ball_L", "ball_R", "lh_ik_driver", "weapon_hand_R", "ankle_L"
#define EXTRA_BONES_PROCCESSING         "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C 80"
// disable jiggle bones without forcing convar
// better to use live debugger to find it by easier way
#define BUILD_TRANSFORMATIONS           "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 28 8B"
// #STR: "view angles", "Pitch: %6.1f   Yaw: %6.1f %38s", "ideal angles", "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %19s", "camera offset", "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %16s", "sv_cheats"
#define CAM_THINK                       "85 C0 75 30 38 87"
// "#empty#", "#int#"
// also easy to find as 'KeyValuesSystem' should be generated in ida as externs
#define KEY_VALUES_FROM_STR             "55 8B EC 83 E4 F8 81 EC 0C 05"
// found by checking what's inside bone threads
// str anything like bone access
// allocation done here? direct reference: 55 8B EC 83 EC 08 56 8B F1 8A 86
#define CACHED_BONE                     "FF B7 ? ? ? ? 52"
// #STR: "CParticleCollection::Simulate", "Particle Simulation"
#define PARTICLE_SIMULATE               "55 8B EC 83 E4 F8 83 EC 30 56 57 8B F9 0F 28 E1 8B 0D ? ? ? ? F3 0F 11 64 24 ? 89 7C 24 18 8B 81"
// #STR: "DispatchEffect: effect \"%s\" not found on client\n", "Encountered multiple different effects with the same nam
// see what calls this and you can see head ptr, should be close to actual message
#define HEAD_OF_EFFECTS                 "8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F 1F 00"
// check CTraceFilterOmitPlayers
#define GOES_THROUGH_SMOKE              "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"
// #STR: "TempEntity", "TE_DispatchEffect %s %s", "originx", "originy", "originz", "startx", "starty", "startz", "normalx", "normaly"
#define DISPATCH_EFFECT                 "55 8B EC 83 E4 F8 83 EC 20 56 57 8B F9 C7 44 24 ? ? ? ? ? 8D 4C 24 08 C7 44 24 ? ? ? ? ? 8B F2 C7 44 24"
// #STR: "effects/tesla_glow_noz"
#define FX_TESLA                        "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 47 18"
// #STR: "effects/flashlight001", "Other textures"
#define FLASHLIGHT_CREATE               "55 8B EC F3 0F 10 45 ? B8"
// #STR: "entindex", "flashlightHandle", "flashlightState", "FlashlightState"
#define FLASHLIGHT_DESTROY              "56 8B F1 E8 ? ? ? ? 8B 4E 28"
// #STR: "CFlashlightEffect::UpdateLight", "FlashlightState", "entindex", "flashlightHandle", "flashlightState", "Flashlight Shadows"
#define FLASHLIGHT_UPDATE               "55 8B EC 81 EC ? ? ? ? 53 56 8B F1 8B 0D ? ? ? ? 57 8B 81"
// my method is to first lookup the const CClientState::`vftable'
// we can see this function: direct reference: [actual address in first opcode] E8 ? ? ? ? EB 02 33 F6 85 FF
// a lot of initialization going on here, but most importantly we must find the pointer to clientstate
// skip reading to the last memset operation, see mov eax, edi -> this copies to the result
// first operation below is the offset to add. This although is very stupid method, better is this:
// "CEngineClient::SetViewAngles:  rejecting invalid value [%f %f %f]\n" see what is called under
// currently dword_1059F194, now to get it, there are many functions containing it. This sig needs small opcode add.
#define CLIENT_STATE                    "A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07"
// STR: "CNetChan_TransmitBits->send", "CNetChan::SendDatagram
#define SEND_DATAGRAM                   "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 14"
// STR: const CTraceFilterSimple
#define CTRACE_FILTER_SIMPLE            "55 8B EC 83 E4 F0 83 EC 7C 56 52"
// #STR: "CHudWeaponSelection"
#define CSGO_HUD                        "B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89"
// #STR: "[%d] Could not find Hud Element: %s\n"
#define FIND_ELEMENT                    "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28 7E 1E"
// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/studiorender/r_studiodraw.cpp#L451
// inside: direct reference: [actual address in first opcode] E8 ? ? ? ? 8B 4D 2C
// for ( i = this; v12 < a4; v11 += R_StudioDrawPoints((int)i, a2, a3, a6, a7, a8, v18, a10, a11) )
// need to get now mov instruction for ebp
// got it from here: https://www.unknowncheats.me/forum/counterstrike-global-offensive/125029-d3d-chams-without-vertcounts.html
#define R_STUDIODRAWPOINTS              "8B 7D FC 03 F8 89 7D FC EB 1F"
// to find it you must check it yourself
// anything like Failed to convert image data to RGBA might help to path it
#define UNK_FILESYS                     "C7 43 ? ? ? ? ? 83 7C 24 ? ? 7C 25 8B 54 24 48"
// #STR: "Error! CMapOverview::SetMap: couldn't load file %s.\n", "file://{images_overviews}/%s_radar_spectate.dds", "pos_x", "pos_y", "scale", "resource/overviews/%s.txt"
#define UNK_OVERVIEWMAP                 "55 8B EC 81 EC ? ? ? ? 53 8B 5D 08 8B D3"
// #STR: "ClientScheme", "resource/ClientScheme.res", "VPhysics031", "VPhysicsSurfaceProps001", "VPhysicsCollision007", "ClientDLL"
// get this ptr https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/cdll_client_int.cpp#L1323
#define VIEW_RENDER                     "8B 0D ? ? ? ? FF 75 0C 8B 45 08"
// #STR: "Client_Animation", "C_BaseAnimating::SetupBones", "*** ERROR: Bone access not allowed (entity %i:%s)\n", "%d:%s", "SetupBones: invalid bone array size (%d - needs %d)\n", "Client_Animation_Threaded"
#define ACCUMULATE_LAYERS               "84 C0 75 0D F6 87"
#define SETUP_VELOCITY                  "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80"
#define SETUP_OCCLUSION                 "84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7"
// #STR: "Tried BeginRenderTargetAllocation after game startup. If I
#define DISABLE_RENDER_TARGET_ALLOC		"80 B9 ? ? ? ? ? 74 0F"
// #STR: "userid", "entindex", "weaponhud_selection"
#define LOCAL_PLAYER                    "8B 0D ? ? ? ? 83 FF FF 74 07"
// from prediction stack
#define PREDICTION_MOVE_DATA            "A1 ? ? ? ? F3 0F 59 CD"
// #STR: "Client: Missing precache for particle system \"%s\"!\n" - this func is a mess to start working on rain manually
#define DISPATCH_PARTICLE               "55 8B EC 83 E4 F8 83 EC 6C 53 56 57 8B F9 8B DA 8D 4C 24 10 E8"
// see what's under:
// ; #STR: "$c0_x", "_rt_FullFrameFB", "_rt_SmallFB0", "Other textures", "dev/blurgaussian_3x3", "_rt_SmallFB1", "$c0_y", "$c1_x", "$c1_y", "RenderTargets"
// is called
#define IS_DEPTH                        "8B 0D ? ? ? ? 56 8B 01 FF 50 34 8B F0 85 F6 75 04"
// wrapper to allow in other hooks
// #STR: "effects/underwater_overlay", "$c0_x", "$c0_y", "$c0_z", "ClientEffect textures"
#define DRAW_EFFECT_MATRIAL             "55 8B EC 83 E4 ? 83 EC ? 53 56 57 8D 44 24 ? 89 4C"
// #STR: "StartParticleEffect:  Failed to find precached particle sy
#define START_PARTICLE_EFFECT           "55 8B EC 83 EC 34 80 3D ? ? ? ? ? 53 56 57 8B DA 8B F1 0F 85"
// #STR: "state", "ParticleSystem_Destroy"
#define PARTICLE_SYSTEM_DESTROY         "56 8B F1 F6 86 ? ? ? ? ? C7 06"
// #STR: "Attempted to create unknown particle system type \"%s\"!\n, "error", "explosion_smokegrenade"
// see what under this str gets called
#define PARTICLE_CALL                   "55 8B EC 83 EC ? 53 56 8B F2 89 75"
// found by following leak's code again use unknown particle as str ref
#define IS_EFFECT_CACHED                "55 8B EC 83 EC ? 53 56 57 8B 7D ? 8D 59 ? 85 FF"
// #STR: "state", "ParticleSystem_SetControlPointPosition"
#define SET_PARTICLE_POINT              "55 8B EC 53 8B 5D ? 56 8B F1 F6 86"
// explosion_smokegrenade, check inside: unknown particle
#define PARTICLE_SYSTEM                 "55 8B EC 51 56 8B 35 ? ? ? ? 8B CE"
// followed by leak's code
#define FIND_STRING_INDEX               "55 8B EC 83 EC ? 53 8B 5D ? 57 8B F9 89 7D ? 85 DB"
// #STR: "Particle/Effect_Rendering", "FX_Blood"
#define FX_BLOOD                        "55 8B EC 83 EC 34 53 56 57 8B F9 F3 0F"
// #STR: "$MotionBlurViewportInternal", "$MotionBlurInternal"
// you can see there is an offset with pointer that is pushed below, "push 4" is helpful, this is the part of editing vector
#define BLUR_MATERIAL_ARR_1             "68 ? ? ? ? FF 50 34 8B 4E 08 5E 85 C9 74 0C"
// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/cstrike15/fx_cs_blood.cpp#L381
#define BLOOD_CALLBACK                  "55 8B EC 8B 4D 08 F3 0F 10 51 ? 8D 51 18"
// old was in VClientEntityList0 exactly like
// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/cliententitylist.h#L130
// now can't get addr by index, they are placed exactly in same place tho
// there is now one class for cache - it only has 2 vfuncs. Same way like apex has and all updated source engine games
// sig for the new class ptr B9 ? ? ? ? E8 ? ? ? ? 8B C8 E8 ? ? ? ? 8B F0 8B CE
#define ADD_ENT                         "55 8B EC 51 8B 45 0C 53 56 8B F1 57"
#define REMOVE_ENT                      "55 8B EC 51 8B 45 0C 53 8B D9 56 57 83 F8 FF 75 07"
// #STR: "CPrediction::ProcessMovement", and see what calls random seed by 0FFFFFFFF, at the bottom the jump is predicted player
#define PREDICTED_PLAYER                "89 35 ? ? ? ? F3 0F 10 48 20"
// #STR: "CLIENT:  %s(%s) thinking for %.02f ms!!!\n", ".PAVC_BaseEntity@@"
#define PHYSICS_RUN_THINK               "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87"
// #STR: CPrediction::ProcessMovement
// and leak RunCommand
#define BUTTONS_FORCED                  "8B 86 44 33 ? ? 09 47 30"
// #STR: "CPrediction::ProcessMovement"
// and leak RunCommand
#define LAST_COMMAND                    "8D 8E ? ? ? ? 89 5C 24 3C"
// found by looking around in C_BaseEntity, I looked in mac binaries and compared current one
#define IS_FOLLOWED_ENT                 "F6 ? ? ? ? ? ? 74 31 80"
// #STR: "cl_updaterate"
#define RET_ADDR_INTERPOLATION          "84 C0 0F 85 ? ? ? ? 38 05 ? ? ? ? 0F 84 ? ? ? ? 53"
// #STR: CPrediction::ProcessMovement
// and leak RunCommand
#define POST_THINK_PHYSICS              "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB"
// #STR: CPrediction::ProcessMovement
// and leak RunCommand
#define SIMULATE_ENTITIES               "56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74"
// #STR: "CCSPlayer", "CPlantedC4", "CHostage", "Unknown entity update received by ProcessSpottedEntityUpda, "Unknown entity class received in ProcessSpottedEntityUpdat
#define SPOTTED_ENTITIY_UPDATE          "55 8B EC 83 EC 18 8B 45 08 53 56 57"
// "\ntime\tbullet\trange\trecovery\tinaccu"
// xref addtoTail() function or follow player stack direct reference: [actual address in first opcode] E8 ? ? ? ? 46 3B B3 ? ? ? ? 7C 8C
#define VEC_CLIENT_IPACT_LIST           "8D 8F ? ? ? ? F3 0F 10 84 24"
// #STR: "Server event \"%s\", Tick %i:\n", "GameEventListener2 callback in list that should NOT be - %, "Callback for event \"%s\" is NULL!!!\n", "portal2", "FireEvent: event '%s' not registered.\n", "Game event \"%s\", Tick %i:\n"
#define FIRE_INTERN                     "55 8B EC 83 E4 F8 83 EC 0C 8B C1 53 56"
// #STR: "KeyValues::LoadFromBuffer(%s%s%s): Begin"
#define LOAD_FROM_BUFFER                "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"
// #STR: "KeyValues::ParseIncludedKeys: Couldn't load included keyva
#define KEYVALUES_CONSTRUCT             "55 8B EC 56 8B F1 33 C0 8B 4D 0C 81"
// #STR: "KeyValues::ParseIncludedKeys: Couldn't load included keyva
#define KEYVALUES_DESTRUCT              "56 8B F1 E8 ? ? ? ? 8B 4E 14"
// #STR: CPrediction::ProcessMovement
#define USING_STANDARD_WEAPONS_VEH      "56 57 8B F9 8B 97 ? ? ? ? 83 FA FF 74 43 0F"
// #STR: C_BasePlayer::`vftable
#define SELECT_ITEM                     "55 8B EC 56 8B F1 ? ? ? 85 C9 74 71 8B 06"
// #STR: CPrediction::ProcessMovement
#define CHECK_HAS_THINK_FN              "55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B"
// #STR: "C_BaseEntity::SaveData"
// just after first offset has been pushed with "SaveData", there is a copy
#define TRANSFER_DATA                   "55 8B EC 8B 45 10 53 56 8B F1 57"
// #STR: "Shutdown %i predictable entities and %i client-created ent
#define SHUTDOWN_PREDICTABLES           "53 56 8B 35 ? ? ? ? 33 DB 57 33 FF"
// #STR: "%d:  Reinitialized %i predictable entities\n"
#define REINIT_PREDICTABLES             "A1 ? ? ? ? B9 ? ? ? ? 53 56 FF 50 18"
// #STR: "userid", "gg_halftime"
#define SERVER_PREROUND                 "55 8B EC 83 EC 0C 53 8B D9 56 57 8B 03"
// #STR: "Can't find specific footstep sound! (%s) - Using the defau, "Heavy.Step", "T_Default.Suit", "CT_Default.Suit", "ct_%s"
#define PLAY_STEP_SOUND                 "55 8B EC 8B 45 18 81 EC"
// #STR: "userid", "health", "priority", "player_hurt", "attacker"
#define ON_TAKE_DMFG_ALIVE              "55 8B EC 83 EC 18 56 57 8B 7D 08 8B F1 57"
// #STR: "Rain simulation: %du (%d tracers)"
#define SIMULATE_PARTICLE               "55 8B EC 83 E4 F8 83 EC 1C 53 8B D9 F3 0F 11 4C 24"
// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/shared/physics_main_shared.cpp#L1783
#define SIMULATION_TICK                 "8B 86 AC 02"
// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/c_baseplayer.cpp#L2906
#define COMMAND_CONTEXT                 "C6 86 0C 35"
// #STR: "Reference Count for Material %s (%d) != 0\n"
#define MATERIAL_DESTRUCT               "55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 56 8B F1 B9"
// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/cstrike15/cs_hud_scope.cpp#L126
#define SCOPE_PAINT                     "55 8B EC 83 E4 F8 83 EC 78 56 89 4C 24 14"
// #STR: "C_BaseEntity::RestoreData"
#define RESTORE_DATA                    "55 8B EC 83 E4 F8 83 EC 44 53 56 8B 75 0C 8B"
// #STR: "C_BaseEntity::SaveData"
#define SAVE_DATA                       "55 8B EC 83 E4 F8 83 EC 40 56 8B 75 0C 57 8B"
// bloodspray or something, those callbacks straight up never get called
#define BLOOD_UTIL_CALLBACK             "56 57 8B FA 8B F1 83 FF FF 0F 84 ? ? ? ? 85 FF 75 0C A1 ? ? ? ? B9 ? ? ? ? EB 0A"
// #STR: "hidehud"
#define CHUD_IS_HIDDEN                  "55 8B EC 83 EC 10 80 79 7C 00"
// #STR: "effects/flashbang", "$basetexture", "effects/flashbang_white", "ClientEffect textures"
#define FLASH_EFFECT_RET                "FF 50 74 83 EF 01"
// #STR: "effects/flashbang", "$basetexture", "effects/flashbang_white", "ClientEffect textures"
#define FLASH_EFFECT_WHITE_RET          "88 4D F4 8B 0D"
// #STR: "ViewDrawFade", "World Rendering"
#define VIEW_FADE                       "55 8B EC 8B 0D ? ? ? ? 56 8B B1"
// #STR: "effects/blood_gore", "blooddrops", "effects/blood_drop", "effects/blood_core"
#define FX_BLOOD_SPRAY                  "55 8B EC 81 EC ? ? ? ? 53 8B DA F3 0F 11 55"
// puprose is to never use events and get ptr to it from here, ez
// #STR: "duration", "AvatarSpec__HealthBar_PulseOnce", "show_survival_respawn_status", "loc_token", "#Survival_Respawn_Countdown", "player_hurt", "userid"
#define UNK_SURVIVAL_RELATED            "55 8B EC 51 53 8B 5D 08 56 57 8B F9 8B CB 8B 03 89 7D FC FF 50 04 8B D0 BE ? ? ? ? 0F 1F 00 8A 08 3A 0E 75 1A 84 C9 74 12 8A 48 01 3A 4E 01 75 0E 83 C0 02 83 C6 02 84 C9 75 E4 33 C0 EB 05 1B C0 83 C8 01 85 C0 0F 85 ? ? ? ?"
// #STR: "GameInstructor", "C_GameInstructor::FireGameEvent", "gameinstructor_draw", "Round ended...\n", "Set to draw...\n", "gameinstructor_nodraw", "Set to not draw...\n", "round_end", "hltv_replay", "GAME INSTRUCTOR:"
#define GAME_INSTRUCTOR_EVENT           "55 8B EC 83 EC 08 53 56 57 8B F9 8B 0D ? ? ? ? 8B 81 ? ? ? ?"
// #STR: "snow_outer", "rain_storm", "rain_storm_screen", "rain_storm_outer", "rain_outer", "ash_outer"
#define PRECIPITATION_INIT              "55 8B EC 83 EC 5C 8B C1"
// #STR: "Rain simulation: %du (%d tracers)"
#define CLIENT_THINK_PRECIPITATION      "A1 ? ? ? ? 83 C1 F4"
// very, very many places. I picked the one with this str ref:
// #STR: "origin: <%d, %d, %d>\n", "light:  %d\n"
#define HOST_STATE                      "A1 ? ? ? ? 83 EC 08 56 BE ? ? ? ?"
// found from glow DrawModel function
#define FIRST_MOVE_CHILD                "8B 80 0C 03 00 00 83 F8 FF 0F 84 C2 00 00 00"
// found from glow DrawModel function
#define NEXT_PEER                       "8B 8E ? ? ? ? 83 F9 FF 74 1F 0F B7 C1 C1 E0 04 05 ? ? ? ? 74 0C C1 E9 10 39 48 04 75 04 8B 30 EB 02 33 F6 85 F6 75 B5"
// #STR: "func_breakable", "func_breakable_surf"
#define TAKE_DMG                        "80 BE ? ? ? ? ? 75 46 8B 86 ? ? ? ? 83 F8 11"
// below other setabs... functions, as a reference take anything in client that uses it
#define SET_ABS_VELOCITY                "55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1 F3 0F 10 07 0F 2E 86 ? ? ? ? 9F F6 C4 44 7A 28 F3 0F"
// #STR: "Client: Missing precache for particle system \"%s\"!\n"
#define GET_PARTICLE_SYSTEM_IDX         "56 8B F1 85 F6 74 22 8B 0D ? ? ? ?"
// #STR: "Client: Missing precache for particle system \"%s\"!\n"
#define DISPATCH_PARTICLE_EFFECT        "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 8B F1 85 F6"
// spotted in the glow
#define RENDER_BOX                      "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 57 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 89 74 24 2C 85 F6 74 07 8B 06"
// close to render box
#define RENDER_LINE                     "55 8B EC 81 EC ? ? ? ? 53 56 57 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 F6 74 07 8B 06"
// CEnginePostMaterialProxy::OnBind
#define MATERIALPROXY_ONBIND            "56 8B F1 57 8B 4E 04 85 C9"
// ClientModeCSNormal::UpdatePostProcessingEffects
// https://www.unknowncheats.me/forum/2019182-post26.html
#define BLURSCOPE_VIGNETTEPOST          "0F 11 05 ? ? ? ? F3 0F 7E 87"
// ClientModeCSNormal::UpdatePostProcessingEffects()
#define UPDATEPOST_EFFECTS              "55 8B EC 51 53 56 57 8B F9 8B 4D 04 E8 ? ? ? ? 8B 35 ? ? ? ? 85 F6"
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/331329-achieving-noscope-blurry-lines.html
#define SCOPE_DUST_RET                  "FF 50 3C 8B 4C 24 20"
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/331329-achieving-noscope-blurry-lines.html
#define SCOPE_ARC_RET                   "8B 0D ? ? ? ? FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 7C 24 1C"
// refer to other glow sigs, or glow_outline code
#define ADD_GLOW_BOX                    "55 8B EC 53 56 8D 59"
// #STR: "C_BaseAnimating::DrawModel"
#define BASE_ANIMATING_DRAW_MODEL       "55 8B EC 83 EC 10 53 56 8B F1 8B 0D ? ? ? ? 57 89 75 F8 8B 81 ? ? ? ?"
// find any place in sdk that setString is used, often material inits
#define KEY_VALUES_SET_STRING           "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01 FF 77 04"
// same...
#define KEY_VALUES_FIND_KEY             "55 8B EC 83 EC 1C 53 8B D9 85 DB"
// follow refs as some effects, eg: embers
#define GET_PMATERIAL                   "55 8B EC 83 EC 18 53 8B D9 56 57 83 7B 60 00 75 0B"
// r_printdecalinfo
#define S_ADECALPOOL                    "A1 ? ? ? ? 8B 0C B0"
// "psurface NULL in R_DecalCreate!\n"
#define R_DECAL_CREATE                  "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 38 8B C2 F3 0F 11 5D ? F3 0F 11 55 ? 89 45 FC"
// reverse decal adding
#define R_DECAL_ADD_TO_SURFACE          "55 8B EC 83 EC 08 53 56 8B D9 B9 ? ? ? ? 57"
// reverse init decals
#define G_ADECALSORTPOOL                "B9 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? B9 ? ? ? ? FF 05 ? ? ? ?"
// #STR: "Couldn't get trampoline region lock, will continue possibl
#define VALVE_HOOK                      "55 8B EC 51 8B 45 10 C7"
// #STR: "Aborting UnhookFunc because pRealFunctionAddr is null\n", "System page size: %u\n", "UnhookFunc not restoring original bytes - jump instruction, "UnhookFunc not restoring original bytes - jump target has , "Warning: VirtualProtect call failed during unhook\n", "Warning: VirtualProtect (2) call failed during unhook\n", "Warning: VirtualProtect (2) call failed to restore protect, "Warning: VirtualProtect call failed to restore protection , "UnhookFunc not restoring original bytes - function is unma, "Aborting UnhookFunc because pRealFunctionAddr is not hooke
#define VALVE_UNHOOK                    "55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 81 EC ? ? ? ? 56 8B 75 08 85 F6 75 26 80 7D 0C 00 0F 84 ? ? ? ?"
// saw in the renderoverlaysmoke
#define VIEW_FACE_SMOKE_RET             "F3 0F 2C C0 88 45 FF 8B 01 FF 50 74"
// r_flashlightdepthtexture check at end
#define VIEW_DRAW_SCENE                 "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 10 A1 ? ? ? ? 56 8B F1 57 8B 40 04 48"
// follow effects sode for precip
#define CREATE_PARTICLE_PRECIP          "55 8B EC 51 A1 ? ? ? ? 53 56 8B F1 B9 ? ? ? ?"
// follow effects sode for precip
#define INITIALIZE_PRECIP_PARTICLE      "55 8B EC 83 EC 5C 8B C1"
// #STR: "CEffectsList::DrawEffects", "Particle/Effect_Rendering"
#define DRAW_EFFECTS                    "55 8B EC 83 EC 08 A1 ? ? ? ? 89 4D FC 8B 90 ? ? ? ?"
// #STR: "$depth_feather"
#define DRAW_WORLD_AND_ENTITIES         "55 8B EC 83 EC 4C 53 56 57 8B 3D ? ? ? ? 8B F1 8B CF 89 75 F8 8B 07 FF 90 ? ? ? ? 8B 0D ? ? ? ? 8D 45 B8 8B 5D 0C 50 6A 00"
// #STR: "DrawTranslucentEntities"
#define DRAW_TRANSCULENT_RENDERABLE     "55 8B EC 81 EC ? ? ? ? 83 3D ? ? ? ? ? 53 56 8B D9 57 89 5D E4 74 1C 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 6A 00"
// saw in the renderview where precip loop is executed (inlined call though)
#define PRECIPICATION_UNK_COUNTER       "3B 35 ? ? ? ? 7C E9 A1 ? ? ? ? B9 ? ? ? ? 8B 40 34"
// ^ like so
#define G_PRECIPICATION                 "A1 ? ? ? ? 8B 0D ? ? ? ? 8B 04 B0 0F BF 90 ? ? ? ? 8B 01 52 FF 50 18 85 C0 0F 84 ? ? ? ? B9 ? ? ? ? 66 85 08 0F 86 ? ? ? ? 8B 0D ? ? ? ? 8D 5D F4 8B 40 04 53 8B 11"
// string ref "World" strcpy
#define CLIENT_WORLD                    "A1 ? ? ? ? F3 0F 10 4D ? EB 71"
// see the g_precips vec, when it removes element
#define DESTRUCT_PRECIPICATION          "55 8B EC 51 56 8B F1 8D 45 FC 57"
// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/c_baseentity.cpp#L1189
#define DORMANT_STATE                   "66 C7 86 ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? 66 89 86 ? ? ? ? 89 86 ? ? ? ?"
// check strings used for stop in leak
#define STOP_SOUND                      "55 8B EC 51 56 57 8B F9 8B 47 08 8D 4F 08 FF 50 28 83 F8 FF 75 1A 8B 07"
// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/client/cstrike15/clientmode_csnormal.cpp#L2246
// lot of events
#define CLIENTMODE_CSNORMAL_EVENT       "55 8B EC 83 E4 C0 B8 ? ? ? ? E8 ? ? ? ? 89 4C 24 08"
// reversed rain draw
#define TRACER_DRAW                     "55 8B EC 83 EC 40 8B C2 8B 55 08 0F 28 D3 56 8B F1 8D 4D C0 51 8B C8 E8 ? ? ? ? 83 C4 04"
// reversed weather code
#define WEATHER_GETVCOLLIDE_RET         "83 0F 02 83 0B 02 39 35 ? ? ? ? 0F 8E ? ? ? ? A1 ? ? ? ? 8B 0D ? ? ? ? 8B 04 B0 0F BF 90 ? ? ? ? 8B 01 52 FF 50 18"
// reversed weather code
#define DISPATCH_INNER_PRECIP           "55 8B EC 83 EC 40 53 56 57 8B 7D 08 8B F1 57"
// #STR: "Attempting to create unknown particle system '%s' \n"
#define NEW_PARTICLE_EFFECT_CREATE      "55 8B EC 51 53 56 8B 35 ? ? ? ? 57 8B 7D 08 89 4D FC 8B CE 57 E8 ? ? ? ? 84 C0"
// #STR: "ParticleSystem_SetControlPointPosition"
#define NEW_PARTICLE_SET_POINT          "55 8B EC 53 8B 5D 0C 56 8B F1 F6 86 ? ? ? ? ? 0F 84 ? ? ? ?"
// #STR: "ParticleSystem_SetControlPointObject"
#define NEW_PARTICLE_SET_POINT_ENT      "55 8B EC 53 56 8B F1 57 8B 7D 0C F6 86 ? ? ? ? ? 0F 84 ? ? ? ?"
// reversed weather code, dispatch
#define SET_CONTROL_POINT_WEATHER       "8D 45 C0 50 6A 01 E8 ? ? ? ?"
// reversed physics
#define GET_VELOCITY                    "FF 90 ? ? ? ? F3 0F 10 8C 24 ? ? ? ? F3 0F 10 84 24 ? ? ? ?"
// #STR: "CTempEnts::PhysicsProp: model index %i not found\n"
#define VPHYSICS_GET_OBJ                "8B 8E ? ? ? ? 85 C9 74 3A"
// "Can't force single thread from within t"...
#define FORCE_SINGLE_THREADED           "53 56 57 8B F9 FF 15 ? ? ? ? 84 C0 75 0E"
// reversed engine models
#define RESTOTRE_MATERIALS              "55 8B EC 83 E4 F8 83 EC 08 8B 0D ? ? ? ?"
// "Mod_LoadCubemapSamples: funny lump size"
#define CUBEMAP_LOAD                    "55 8B EC 81 EC ? ? ? ? 53 56 57 51 6A 2A 8D 8D ? ? ? ? E8 ? ? ? ?"
// "CMaterial::PrecacheVars: error loading vmt file for %s\n"
#define PRECACHE_VARS                   "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14 53 8B D9 56 8B 75 10 89 5D EC 57 8D 7B 50 85 F6"
// find yourself, scan the game
#define LEVEL_NAME                      "80 3D ? ? ? ? ? 74 ? 8A 08 80 F9 ? 74 ? 80 F9 ? 75 ? C6 00 ? 40 80 38 ? 75 ? 8A 45"
// "ClientDll"
#define INIT_GAME_SYSTEMS               "55 8B EC 83 EC 08 57 8B F9 89 7D FC E8 ? ? ? ? 84 C0"