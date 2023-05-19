#pragma once

#include <Windows.h>

#include <utilities/cStr.hpp>
#include <utilities/tools/wrappers.hpp>

#include <unordered_map>
#include <lazy_importer.hpp>
#include <string>

namespace memory
{
	namespace modules
	{
		inline std::unordered_map<std::string_view, HMODULE> allmodules;

		template<cStr str>
		inline void addModule()
		{
			allmodules[std::string_view{ str.c_str() }] = reinterpret_cast<HMODULE>(::li::detail::lazy_module<hasher(str.c_str())>().cached());
		}
		inline HMODULE getModule(const std::string_view str)
		{
			return allmodules.at(str);
		}
	}
}

#define ENGINE_DLL					"engine.dll"
#define CLIENT_DLL					"client.dll"
#define VSTD_DLL					"vstdlib.dll"
#define VGUI_DLL					"vgui2.dll"
#define VGUIMAT_DLL					"vguimatsurface.dll"
#define MATERIAL_DLL				"materialsystem.dll"
#define LOCALIZE_DLL				"localize.dll"
#define STUDIORENDER_DLL			"studiorender.dll"
#define INPUTSYSTEM_DLL				"inputsystem.dll"
#define SHARED_API					"shaderapidx9.dll"
#define TIER_DLL					"tier0.dll"
#define PANORAMA_DLL				"panorama.dll"
#define FILESYS_DLL					"filesystem_stdio.dll"
#define DATACACHE_DLL				"datacache.dll"
#define GAME_OVERLAY				"gameoverlayrenderer.dll"