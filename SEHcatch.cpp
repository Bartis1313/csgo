#include "SEHcatch.hpp"

#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

#include <DbgHelp.h>
#include <unordered_map>

// exception to string
#define E2S(e) { e, #e }

#pragma region crash_map
const std::unordered_map<DWORD, const char*> crashNames
{
	E2S(EXCEPTION_ACCESS_VIOLATION),
	E2S(EXCEPTION_READ_FAULT),
	E2S(EXCEPTION_DATATYPE_MISALIGNMENT),
	E2S(EXCEPTION_BREAKPOINT),
	E2S(EXCEPTION_SINGLE_STEP),
	E2S(EXCEPTION_ARRAY_BOUNDS_EXCEEDED),
	E2S(EXCEPTION_FLT_DENORMAL_OPERAND),
	E2S(EXCEPTION_FLT_DIVIDE_BY_ZERO),
	E2S(EXCEPTION_FLT_INEXACT_RESULT),
	E2S(EXCEPTION_FLT_INVALID_OPERATION),
	E2S(EXCEPTION_FLT_OVERFLOW),
	E2S(EXCEPTION_FLT_STACK_CHECK),
	E2S(EXCEPTION_FLT_UNDERFLOW),
	E2S(EXCEPTION_INT_DIVIDE_BY_ZERO),
	E2S(EXCEPTION_INT_OVERFLOW),
	E2S(EXCEPTION_PRIV_INSTRUCTION),
	E2S(EXCEPTION_NONCONTINUABLE_EXCEPTION),
	E2S(EXCEPTION_STACK_OVERFLOW),
	E2S(EXCEPTION_INVALID_DISPOSITION),
	E2S(EXCEPTION_GUARD_PAGE),
	E2S(EXCEPTION_INVALID_HANDLE)
};
#pragma endregion

#undef E2S

void SEHcatch::printStack()
{
	void* stack[MAX_LENGTH];
	const auto process = LI_FN_CACHED(GetCurrentProcess)();
	LI_FN_CACHED(SymInitialize)(process, NULL, TRUE);

	const auto frames = LI_FN_CACHED(CaptureStackBackTrace)(NULL, MAX_FRAMES, stack, nullptr);
	const auto symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(CHAR), 1);
	symbol->MaxNameLen = MAX_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (int i = 3; i < frames; i++) // skip 3 records which are useless to print, you might call some other walkstack functions to get more info
	{
		LI_FN_CACHED(SymFromAddr)(process, reinterpret_cast<DWORD64>(stack[i]), NULL, symbol);
		console.log(TypeLogs::LOG_ERR, XOR("{}: {} - 0x{:X}"), frames - i - 1, symbol->Name, symbol->Address);
	}
	free(symbol);
}

LONG WINAPI SEHcatch::memErrorCatch(EXCEPTION_POINTERS* pExceptionInfo)
{
	const auto code = pExceptionInfo->ExceptionRecord->ExceptionCode;

	for (const auto& [crashCode, crashName] : crashNames)
	{
		if (code == crashCode)
		{
			static bool bOnce = [=]()
			{
				const auto symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(CHAR), 1);
				symbol->MaxNameLen = MAX_LENGTH;
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

				const auto addr = pExceptionInfo->ExceptionRecord->ExceptionAddress;
				const auto name = symbol->Name;
				const auto flag = pExceptionInfo->ExceptionRecord->ExceptionFlags;
				const auto params = pExceptionInfo->ExceptionRecord->NumberParameters;

				const auto info = FORMAT(XOR("Exception (fatal) {} at address {} ({}), flags - {}, params - {}"), crashName, addr, name, flag, params);
				// x86
				console.log(TypeLogs::LOG_ERR, XOR("EAX - 0x{:X}"), pExceptionInfo->ContextRecord->Eax);
				console.log(TypeLogs::LOG_ERR, XOR("EBP - 0x{:X}"), pExceptionInfo->ContextRecord->Ebp);
				console.log(TypeLogs::LOG_ERR, XOR("EBX - 0x{:X}"), pExceptionInfo->ContextRecord->Ebx);
				console.log(TypeLogs::LOG_ERR, XOR("ECX - 0x{:X}"), pExceptionInfo->ContextRecord->Ecx);
				console.log(TypeLogs::LOG_ERR, XOR("EDI - 0x{:X}"), pExceptionInfo->ContextRecord->Edi);
				console.log(TypeLogs::LOG_ERR, XOR("EDX - 0x{:X}"), pExceptionInfo->ContextRecord->Edx);
				console.log(TypeLogs::LOG_ERR, XOR("EIP - 0x{:X}"), pExceptionInfo->ContextRecord->Eip);
				console.log(TypeLogs::LOG_ERR, XOR("ESI - 0x{:X}"), pExceptionInfo->ContextRecord->Esi);
				console.log(TypeLogs::LOG_ERR, XOR("ESP - 0x{:X}"), pExceptionInfo->ContextRecord->Esp);

				console.log(TypeLogs::LOG_ERR, info);
				printStack();
				LI_FN(MessageBoxA)(nullptr, info.c_str(), XOR("Fatal error!"), MB_ICONERROR | MB_OK);
				free(symbol);
				return true;
			} ();

			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}