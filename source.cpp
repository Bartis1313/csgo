#include "cheats/hooks/hooks.hpp"
#include "cheats/features/misc/callbacks.hpp"
#include "cheats/hooks/helpers/helper.hpp"
#include "utilities/console/console.hpp"
#include "SDK/interfaces/interfaces.hpp"
#include "utilities/renderer/renderer.hpp"
#include "utilities/netvars/netvars.hpp"
#include "config/vars.hpp"
#include "cheats/globals.hpp"
#include "cheats/features/misc/events.hpp"
#include "utilities/simpleTimer.hpp"
#include "cheats/menu/GUI-ImGui/menu.hpp"
#include "utilities/console/console.hpp"
#include "SEHcatch.hpp"
#include "cheats/menu/x88Menu/x88menu.hpp"
#include "cheats/features/misc/discord.hpp"

#include <thread>

#if !(defined _WIN32)
#error This is only for windows and x86
#endif

using namespace std::literals;

VOID WINAPI _looper(PVOID instance);

DWORD WINAPI init(PVOID instance)
{
    AddVectoredExceptionHandler(TRUE, SEHcatch::memErrorCatch);

    console.init(XOR("CSGO DEBUG"), XOR("hack.log"));

    TimeCount initTimer{};

    // warning: if you do wrong hierarchy - crash
    try
    {
        config.init();
        if (auto name = config.get<std::string>(vars.sLoadName); name != config.getDefaultConfigName()) // custom load
            config.load(name);
        interfaces::init();
        netvarMan.init();
        netvarMan.dump();
        surfaceRender.init();
        x88menu.init();
        hooks::wndProcSys::init();
        callbacks.init();
        callbacks.run();
        hooks::init();
    }
    catch (const std::runtime_error& err)
    {
        LF(MessageBoxA)(nullptr, err.what(), XOR("Runtime hack error"), MB_OK | MB_ICONERROR);
        LF(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_FAILURE);
    }

    initTimer.end();
    console.log(TypeLogs::LOG_INFO, XOR("main thread took {:.5f}s"), initTimer.getSec());

    return TRUE;
}

VOID WINAPI _looper(PVOID instance)
{
    dc.init();
    while (!config.get<Key>(vars.kPanic).isPressed())
    {
        std::this_thread::sleep_for(1s);
        dc.run();
    }

    globals::isShutdown = true;

    RemoveVectoredExceptionHandler(globals::instance);
    hooks::wndProcSys::shutdown();
    events.shutdown();
    callbacks.shutdown();
    hooks::shutdown();
    menu.shutdown();
    console.log(TypeLogs::LOG_INFO, XOR("Hack shutdown"));
    console.shutdown();
    dc.shutdown();

    LF(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_SUCCESS);
}

BOOL WINAPI DllMain(CONST HMODULE instance, CONST ULONG reason, CONST PVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        // here this sometimes throw null on mm
        if (instance)
            LF(DisableThreadLibraryCalls)(instance);

        globals::instance = instance;

        if (auto initThread = LF(CreateThread)(nullptr, NULL, init, instance, NULL, nullptr))
            LF(CloseHandle)(initThread);

        if (auto shutdownThread = LF(CreateThread)(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(_looper), instance, NULL, nullptr))
            LF(CloseHandle)(shutdownThread);

        return TRUE;
    }

    return FALSE;
}