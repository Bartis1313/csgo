#pragma once

#include <Windows.h>

#ifdef _WIN64
#define READ_PEB_POINTER __readgsqword
#else
#define READ_PEB_POINTER __readfsdword
#endif

extern "C"
{
	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING, * PUNICODE_STRING;
	typedef const UNICODE_STRING* PCUNICODE_STRING;

	typedef struct _PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		HANDLE SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
		// ..
	} PEB_LDR_DATA, * PPEB_LDR_DATA;

	typedef struct _LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		union
		{
			LIST_ENTRY InInitializationOrderLinks;
			LIST_ENTRY InProgressLinks;
		};
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		// ...
	} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

	typedef struct _PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PPEB_LDR_DATA Ldr;
		PVOID ProcessParameters; // PRTL_USER_PROCESS_PARAMETERS
		// ...
	} PEB, * PPEB;
}

namespace windows
{
	PPEB getPeb();
	FARPROC getExportAddress(HMODULE hModule, std::string_view exportName);
	HMODULE getModuleHandle(std::string_view moduleName);
}