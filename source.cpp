#include "cheats/hooks/hooks.hpp"
#include "cheats/hooks/helpers/helper.hpp"
#include "utilities/console/console.hpp"
#include "SDK/interfaces/interfaces.hpp"
#include "utilities/renderer/renderer.hpp"
#include "utilities/netvars/netvars.hpp"
#include "utilities/tools/tools.hpp"
#include "config/vars.hpp"
#include "cheats/globals.hpp"
#include "cheats/features/sources/events/events.hpp"
#include "utilities/simpleTimer.hpp"
#include "cheats/menu/GUI-ImGui/menu.hpp"
#include "utilities/console/console.hpp"
#include "SEHcatch.hpp"
#include "cheats/menu/x88Menu/x88menu.hpp"
#include "utilities/res.hpp"
#include "cheats/features/sources/discord/discord.hpp"
#include "cheats/features/base.hpp"
#include "cheats/game.hpp"
#include "gamememory/memory.hpp"

#include <thread>

#if !(defined _WIN32)
#error This is only for windows and x86
#endif

using namespace std::literals;

static bool inited = false;

DWORD WINAPI init(PVOID instance)
{
    AddVectoredExceptionHandler(TRUE, SEHcatch::memErrorCatch);

    console.init(XOR("CSGO DEBUG"), XOR("hack.log"));

    TimeCount initTimer{};

    // warning: if you do wrong hierarchy - crash
    try
    {
        config.init(
            XOR("default.cfg"), XOR("load.LOAD"),
            std::filesystem::path{ XOR("Bartis_internal") } / XOR("csgo"),
            std::filesystem::path{ XOR("Bartis_internal") } / XOR("csgo") / XOR("utility"));
        g_Memory.init();
        game::localPlayer.init();
        interfaces::init();
        netvarMan.init();
        netvarMan.dump();
        surfaceRender.init();
        x88menu.init();
        hooks::wndProcSys::init();
        BaseHack::initAll();
        g_Events.init();
        hooks::init();
    }
    catch (const std::runtime_error& err)
    {
        LI_FN(MessageBoxA)(nullptr, err.what(), XOR("Runtime hack error"), MB_OK | MB_ICONERROR);
        LI_FN(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_FAILURE);
    }

    initTimer.end();
    console.log(TypeLogs::LOG_INFO, XOR("main thread took {:.5f}s"), initTimer.getSec());

    inited = true;

    return TRUE;
}

DiscordPresence dc;

VOID WINAPI _shutdown(PVOID instance)
{
    globals::isShutdown = true;

    RemoveVectoredExceptionHandler(globals::instance);
    hooks::wndProcSys::shutdown();
    BaseHack::shutdownAll();
    Events::shutdownAllEvents();
    Resource::destroyAll();
    hooks::shutdown();
    menu.shutdown();
    console.log(TypeLogs::LOG_INFO, XOR("Hack shutdown"));
    console.shutdown();
    dc.shutdown();

    LI_FN(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_SUCCESS);
}

VOID WINAPI _looper(PVOID instance)
{
    dc.init();
    while (!config.get<Key>(vars.kPanic).isPressed())
    {
        if (inited)
        {
            dc.run();
            std::this_thread::sleep_for(1s);
        }
    }

    _shutdown(instance);
}

BOOL WINAPI DllMain(CONST HMODULE instance, CONST ULONG reason, CONST PVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        // here this sometimes throw null on mm
        if (instance)
            LI_FN(DisableThreadLibraryCalls)(instance);

        globals::instance = instance;

        if (auto initThread = LI_FN(CreateThread)(nullptr, NULL, init, instance, NULL, nullptr))
            LI_FN(CloseHandle)(initThread);

        if (auto looperThread = LI_FN(CreateThread)(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(_looper), instance, NULL, nullptr))
            LI_FN(CloseHandle)(looperThread);

        return TRUE;
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        if (!globals::isShutdown) // then panic key forced shutdown
        {
            if (auto shutdownThread = LI_FN(CreateThread)(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(_shutdown), instance, NULL, nullptr))
                LI_FN(CloseHandle)(shutdownThread);
        }
    }

    return FALSE;
}