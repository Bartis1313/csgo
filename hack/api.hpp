#pragma once

#if defined _DEBUG
#define WANT_EXCEPTIONS
#endif

#if defined WANT_EXCEPTIONS
// nothing
#else
#define JSON_NOEXCEPTION 1
#endif

#if defined WANT_EXCEPTIONS
#include <stdexcept>
#define HACK_CATCH_TYPE std::runtime_error
#else
#define HACK_CATCH_TYPE
#endif

#include <Windows.h>
#include <format>

#if defined WANT_EXCEPTIONS
#define HACK_TRY try
#define HACK_CATCH(exception) catch(exception)
#define HACK_THROW(exception) throw std::runtime_error{ exception }
#else
#define HACK_TRY if(true)
#define HACK_CATCH(exception) if(false)
#define HACK_THROW(exception) { const int res = api::messageBox(std::string{ exception }.c_str(), "Exception throw!", MB_TASKMODAL | MB_ICONERROR | MB_YESNOCANCEL); res; /*do smth with res*/  }
#endif

#define HACK_FASTCALL __fastcall
#define HACK_THISCALL __thiscall
#define HACK_STDCALL __stdcall
#define HACK_CDECL __cdecl

// mark functions implicitly that will execute in dllmain
#define HACK_INIT

extern "C" BOOL WINAPI _CRT_INIT(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

#include <filesystem>

namespace api
{
	inline int messageBox(const char* text, const char* caption, int flags = MB_ICONERROR | MB_OK)
	{
		return MessageBoxA(nullptr, text, caption, flags);
	}

	struct Localization
	{
		std::filesystem::path path;
		std::string defaultConfigName;
		std::filesystem::path utilityPath;
		std::string defaultLoadName;
	};

	inline const Localization localization
	{
		.path = std::filesystem::path{ "Bartis_internal" } / "csgo",
		.defaultConfigName = "default.cfg",
		.utilityPath = "utility",
		.defaultLoadName = "load.LOAD"
	};

	[[nodiscard]] std::filesystem::path getDocumentsPath();
	[[nodiscard]] std::filesystem::path getHackPath();
	[[nodiscard]] std::string getDefaultConfigName();
	[[nodiscard]] std::filesystem::path getExtraLoadPath();
}