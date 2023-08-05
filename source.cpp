#include "setup/setup.hpp"

#include <cheats/game/globals.hpp>

BOOL APIENTRY DllMainEntry(CONST HMODULE instance, CONST ULONG reason, CONST PVOID reserved)
{
    if (!_CRT_INIT(instance, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH)
    {
        globals::instance = instance;

        if (!setup::init(instance))
            return FALSE;

        if (auto looperThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup::looper), instance, NULL, nullptr))
            CloseHandle(looperThread);

        return TRUE;
    }
    else if (DLL_PROCESS_DETACH)
    {
        if (!globals::isShutdown) // then panic key forced shutdown
        {
            if (auto shutdownThread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup::shutdown), instance, NULL, nullptr))
                CloseHandle(shutdownThread);
        }
    }

    return TRUE;
}