#include "setup/setup.hpp"

#include <cheats/game/globals.hpp>

BOOL WINAPI DllMain(CONST HMODULE instance, CONST ULONG reason, CONST PVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        // here this sometimes throw null on mm
        if (instance)
            DisableThreadLibraryCalls(instance);

        globals::instance = instance;

        if (auto initThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup::init), instance, NULL, nullptr))
           CloseHandle(initThread);

        if (auto looperThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup::looper), instance, NULL, nullptr))
           CloseHandle(looperThread);

        return TRUE;
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        if (!globals::isShutdown) // then panic key forced shutdown
        {
            if (auto shutdownThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup::shutdown), instance, NULL, nullptr))
                CloseHandle(shutdownThread);
        }
    }

    return FALSE;
}