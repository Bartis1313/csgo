#include "cheats/hooks/hooks.hpp"
#include "utilities/console/console.hpp"
#include "SDK/interfaces/interfaces.hpp"
#include "utilities/renderer/renderer.hpp"
#include "utilities/netvars/netvars.hpp"
#include "config/config.hpp"
#include "source.hpp"

ULONG WINAPI init(PVOID instance)
{
	//  it can throw it when instant quit, ignore that. It's possible to hook game's detection for all of this
	AddVectoredExceptionHandler(1, memErrorCatch);

	console::init(XOR("CSGO DEBUG"));

	// warning: if you do wrong hierarchy - crash
	try
	{
		utilities::prepareDirectories();
		interfaces::init();
		NetvarManager::g().init();
		NetvarManager::g().dump();
		render::init();
		hooks::init();
		config::init();
	}
	catch (const std::runtime_error& err)
	{
		LF(MessageBoxA)(nullptr, err.what(), XOR("Runtime hack error"), MB_OK | MB_ICONERROR);
		LF(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), 1);
	}
	
	return EXIT_SUCCESS;
}

ULONG WINAPI shutdown(PVOID instance)
{
	hooks::shutdown();
	console::shutdown();
	LF(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), 0);
	return TRUE;
}

BOOL WINAPI DllMain(CONST HMODULE instance, CONST ULONG reason, CONST VOID* reserved) 
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	{
		// here this sometimes throw null on mm
		if (instance)
			LF(DisableThreadLibraryCalls)(instance);

		LF(CreateThread)(nullptr, NULL, init, instance, NULL, nullptr);

		break;
	}

	case DLL_PROCESS_DETACH:
	{
		shutdown(instance);
		break;
	}
	}

	return TRUE;
}
