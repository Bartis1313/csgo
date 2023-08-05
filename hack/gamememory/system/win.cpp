#include "win.hpp"

#include <codecvt>
#include <string_view>
#include <unordered_map>

namespace windows
{
	std::unordered_map<std::string_view, HMODULE> cachedModules;
}

PPEB windows::getPeb()
{
#if defined(_WIN64)
	return reinterpret_cast<PPEB>(READ_PEB_POINTER(0x60));
#else
	return reinterpret_cast<PPEB>(READ_PEB_POINTER(0x30));
#endif
}

FARPROC windows::getExportAddress(HMODULE hModule, const std::string_view exportName)
{
	PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
	if (!pDosHeader)
		return nullptr;

	PIMAGE_NT_HEADERS pNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<BYTE*>(hModule) + pDosHeader->e_lfanew);

	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	PIMAGE_EXPORT_DIRECTORY pExportDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(reinterpret_cast<BYTE*>(hModule) + pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	DWORD* pFuncTable = reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(hModule) + pExportDir->AddressOfFunctions);
	DWORD* pNameTable = reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(hModule) + pExportDir->AddressOfNames);
	WORD* pOrdinalTable = reinterpret_cast<WORD*>(reinterpret_cast<BYTE*>(hModule) + pExportDir->AddressOfNameOrdinals);

	for (DWORD i = 0; i < pExportDir->NumberOfNames; ++i) 
	{
		std::string_view currentName = reinterpret_cast<const char*>(reinterpret_cast<BYTE*>(hModule) + pNameTable[i]);
		if (currentName == exportName) 
		{
			WORD ordinal = pOrdinalTable[i];
			DWORD functionRVA = pFuncTable[ordinal];
			return reinterpret_cast<FARPROC>(reinterpret_cast<BYTE*>(hModule) + functionRVA);
		}
	}

	return nullptr;
}

HMODULE windows::getModuleHandle(std::string_view moduleName)
{
	return GetModuleHandleA(moduleName.data());

	if (auto it = cachedModules.find(moduleName); it != cachedModules.end())
		return it->second;

	const PPEB pPeb = getPeb();
	const PPEB_LDR_DATA pLdrData = pPeb->Ldr;

	PLDR_DATA_TABLE_ENTRY pModuleEntry = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(pLdrData->InLoadOrderModuleList.Flink);

	HMODULE moduleBase = nullptr;
	while (pModuleEntry != nullptr && pModuleEntry->DllBase != nullptr)
	{
		std::wstring moduleNameWide(pModuleEntry->BaseDllName.Buffer, pModuleEntry->BaseDllName.Length / sizeof(wchar_t));
		std::string moduleNameString(moduleNameWide.length(), 0);
		// because warning
		std::transform(moduleNameWide.begin(), moduleNameWide.end(), moduleNameString.begin(), [](wchar_t wc)
			{
				return static_cast<char>(wc);
			});
		if (moduleNameString == moduleName)
		{
			moduleBase = static_cast<HMODULE>(pModuleEntry->DllBase);
		}

		pModuleEntry = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(pModuleEntry->InLoadOrderLinks.Flink);
	}

	cachedModules[moduleName] = moduleBase;

	return moduleBase;
}