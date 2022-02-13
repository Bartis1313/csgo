#include "cheats/hooks/hooks.hpp"
#include "utilities/console/console.hpp"
#include "SDK/interfaces/interfaces.hpp"
#include "utilities/renderer/renderer.hpp"
#include "utilities/netvars/netvars.hpp"
#include "config/config.hpp"
#include "config/vars.hpp" // so vars load
#include "source.hpp"
#include "cheats/features/visuals/chams.hpp"
#include "cheats/globals.hpp"
#include "cheats/menu/GUI/drawing.hpp"
#include "cheats/menu/GUI/Controls/TextInput/TextInput.hpp"
#include <thread>

using namespace std::literals;

VOID WINAPI _shutdown(PVOID instance);

DWORD WINAPI init(PVOID instance)
{
    AddVectoredExceptionHandler(TRUE, memErrorCatch);

    console::init(XOR("CSGO DEBUG"));

    // warning: if you do wrong hierarchy - crash
    try
    {
        interfaces::init();
        config.init();
        netvarMan.init();
        netvarMan.dump();
        render::init();
        GUI::initGui();
        hooks::init();
    }
    catch (const std::runtime_error& err)
    {
        LF(MessageBoxA)(nullptr, err.what(), XOR("Runtime hack error"), MB_OK | MB_ICONERROR);
        LF(FreeLibraryAndExitThread)(reinterpret_cast<HMODULE>(instance), EXIT_SUCCESS);
    }

    return TRUE;
}

VOID WINAPI _shutdown(PVOID instance)
{
    while (!GUI::isKeyDown(VK_DELETE))
    {
        std::this_thread::sleep_for(100ms);
    }

    RemoveVectoredExceptionHandler(globals::instance);

    GUI::menu->shutdown();
    LOG(LOG_INFO, XOR("Hack shutdown"));
    hooks::shutdown();
    console::shutdown();

    LF(MessageBoxA)(nullptr, XOR("Hack shutdown"), XOR("Confirmed hack shutdown"), MB_OK | MB_ICONERROR);
    LF(FreeLibraryAndExitThread)(reinterpret_cast<HMODULE>(instance), EXIT_SUCCESS);
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