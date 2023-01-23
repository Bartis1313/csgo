#include "setup/setup.hpp"

#include <cheats/game/globals.hpp>
#include <utilities/tools/tools.hpp>

BOOL WINAPI DllMain(CONST HMODULE instance, CONST ULONG reason, CONST PVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        // here this sometimes throw null on mm
        if (instance)
            LI_FN(DisableThreadLibraryCalls)(instance);

        globals::instance = instance;

        if (auto initThread = LI_FN(CreateThread)(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup::init), instance, NULL, nullptr))
            LI_FN(CloseHandle)(initThread);

        if (auto looperThread = LI_FN(CreateThread)(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup::looper), instance, NULL, nullptr))
            LI_FN(CloseHandle)(looperThread);

        return TRUE;
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        if (!globals::isShutdown) // then panic key forced shutdown
        {
            if (auto shutdownThread = LI_FN(CreateThread)(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup::shutdown), instance, NULL, nullptr))
                LI_FN(CloseHandle)(shutdownThread);
        }
    }

    return FALSE;
}