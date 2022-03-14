#include "cheats/hooks/hooks.hpp"
#include "cheats/hooks/helpers/helper.hpp"
#include "utilities/console/console.hpp"
#include "SDK/interfaces/interfaces.hpp"
#include "utilities/renderer/renderer.hpp"
#include "utilities/netvars/netvars.hpp"
#include "config/vars.hpp" // so vars load
#include "source.hpp"
#include "cheats/globals.hpp"
#include "cheats/features/misc/events.hpp"
#include "utilities/simpleTimer.hpp"
#include "cheats/menu/GUI-ImGui/menu.hpp"
#include <thread>

using namespace std::literals;

VOID WINAPI _shutdown(PVOID instance);

DWORD WINAPI init(PVOID instance)
{
    AddVectoredExceptionHandler(TRUE, memErrorCatch);

    console::init(XOR("CSGO DEBUG"));

    TimeCount initTimer;
    initTimer.start();

    // warning: if you do wrong hierarchy - crash
    try
    {
        interfaces::init();
        config.init();
        netvarMan.init();
        netvarMan.dump();
        //render.init();
        hooks::wndProcSys::init();
        hooks::init();
    }
    catch (const std::runtime_error& err)
    {
        LF(MessageBoxA)(nullptr, err.what(), XOR("Runtime hack error"), MB_OK | MB_ICONERROR);
        LF(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_FAILURE);
    }

    initTimer.end();
    LOG(LOG_INFO, std::format(XOR("{} {:.5f}ms"), XOR("main thread took"), initTimer.getSec()));

    return TRUE;
}

VOID WINAPI _shutdown(PVOID instance)
{
    while (!utilities::getKey(config.get<int>(vars.iKeyPanic)))
    {
        std::this_thread::sleep_for(100ms);
    }

    RemoveVectoredExceptionHandler(globals::instance);
    hooks::wndProcSys::shutdown();
    ImGuiMenu::shutdown();
    events.shutdown();
    hooks::shutdown();
    LOG(LOG_INFO, XOR("Hack shutdown"));
    console::shutdown();

   // LF(MessageBoxA)(nullptr, XOR("Hack shutdown"), XOR("Confirmed hack shutdown"), MB_OK | MB_ICONERROR);
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

        if (auto shutdownThread = LF(CreateThread)(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(_shutdown), instance, NULL, nullptr))
            LF(CloseHandle)(shutdownThread);

        return TRUE;
    }

    return FALSE;
}