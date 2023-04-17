#include "seh.hpp"

#include <DbgHelp.h>
#include <Psapi.h>

#include <utilities/console/console.hpp>
#include <cheats/game/globals.hpp>

#define PAIRED_FLAG(fl) std::make_pair(fl, #fl)

const std::unordered_map<DWORD, const char*> crashNames
{
	PAIRED_FLAG(EXCEPTION_ACCESS_VIOLATION),
	PAIRED_FLAG(EXCEPTION_READ_FAULT),
	PAIRED_FLAG(EXCEPTION_DATATYPE_MISALIGNMENT),
	PAIRED_FLAG(EXCEPTION_BREAKPOINT),
	PAIRED_FLAG(EXCEPTION_SINGLE_STEP),
	PAIRED_FLAG(EXCEPTION_ARRAY_BOUNDS_EXCEEDED),
	PAIRED_FLAG(EXCEPTION_FLT_DENORMAL_OPERAND),
	PAIRED_FLAG(EXCEPTION_FLT_DIVIDE_BY_ZERO),
	PAIRED_FLAG(EXCEPTION_FLT_INEXACT_RESULT),
	PAIRED_FLAG(EXCEPTION_FLT_INVALID_OPERATION),
	PAIRED_FLAG(EXCEPTION_FLT_OVERFLOW),
	PAIRED_FLAG(EXCEPTION_FLT_STACK_CHECK),
	PAIRED_FLAG(EXCEPTION_FLT_UNDERFLOW),
	PAIRED_FLAG(EXCEPTION_INT_DIVIDE_BY_ZERO),
	PAIRED_FLAG(EXCEPTION_FLT_UNDERFLOW),
	PAIRED_FLAG(EXCEPTION_INT_OVERFLOW),
	PAIRED_FLAG(EXCEPTION_PRIV_INSTRUCTION),
	PAIRED_FLAG(EXCEPTION_NONCONTINUABLE_EXCEPTION),
	PAIRED_FLAG(EXCEPTION_STACK_OVERFLOW),
	PAIRED_FLAG(EXCEPTION_INVALID_DISPOSITION),
	PAIRED_FLAG(EXCEPTION_GUARD_PAGE),
	PAIRED_FLAG(EXCEPTION_INVALID_HANDLE)
};

#undef PAIRED_FLAG

constexpr auto MAX_LENGTH = 1024;
constexpr auto MAX_FRAMES = 1024;

void printStack()
{
	void* stack[MAX_LENGTH];
	const auto process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);

	const auto frames = CaptureStackBackTrace(NULL, MAX_FRAMES, stack, nullptr);
	const auto symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256);
	symbol->MaxNameLen = MAX_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (int i = 3; i < frames; i++) // skip 3 records which are useless to print, you might call some other walkstack functions to get more info
	{
		SymFromAddr(process, reinterpret_cast<DWORD64>(stack[i]), NULL, symbol);
		console::error("{}: {} - 0x{:X}", frames - i - 1, symbol->Name, symbol->Address);
	}
	free(symbol);
}

LONG WINAPI SEH::CATCH(EXCEPTION_POINTERS* pExceptionInfo)
{
	const auto code = pExceptionInfo->ExceptionRecord->ExceptionCode;

	for (const auto& [crashCode, crashName] : crashNames)
	{
		if (code == crashCode)
		{
			const auto addr = pExceptionInfo->ExceptionRecord->ExceptionAddress;

			const auto flag = pExceptionInfo->ExceptionRecord->ExceptionFlags;
			const auto params = pExceptionInfo->ExceptionRecord->NumberParameters;

			MODULEINFO moduleInfo{ };
			K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(nullptr), &moduleInfo, sizeof(MODULEINFO));
			if (addr < globals::instance || addr > globals::instance + moduleInfo.SizeOfImage)
			{
				return EXCEPTION_CONTINUE_SEARCH;
			}

			const auto symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
			const auto name = symbol->Name;
			symbol->MaxNameLen = MAX_LENGTH;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

			console::error("Exception (fatal) {} at address {} ({}), flags - {}, params - {}", crashName, addr, name, flag, params);
			// x86
			console::error("EAX - 0x{:X}", pExceptionInfo->ContextRecord->Eax);
			console::error("EBP - 0x{:X}", pExceptionInfo->ContextRecord->Ebp);
			console::error("EBX - 0x{:X}", pExceptionInfo->ContextRecord->Ebx);
			console::error("ECX - 0x{:X}", pExceptionInfo->ContextRecord->Ecx);
			console::error("EDI - 0x{:X}", pExceptionInfo->ContextRecord->Edi);
			console::error("EDX - 0x{:X}", pExceptionInfo->ContextRecord->Edx);
			console::error("EIP - 0x{:X}", pExceptionInfo->ContextRecord->Eip);
			console::error("ESI - 0x{:X}", pExceptionInfo->ContextRecord->Esi);
			console::error("ESP - 0x{:X}", pExceptionInfo->ContextRecord->Esp);

			printStack();
			const auto reply = LI_FN(MessageBoxA)(nullptr, "Continue?", "Fatal error!", MB_ICONERROR | MB_YESNO);
			free(symbol);

			if (reply == IDOK)
				return EXCEPTION_CONTINUE_SEARCH;

			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}