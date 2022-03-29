#include "SEHcatch.hpp"

#include <DbgHelp.h>
#include <sstream>
#include <map>

#include "utilities/console/console.hpp"
#include "utilities/utilities.hpp"

#pragma region crash_map
const std::map<DWORD, const char*> crashNames
{
	{ EXCEPTION_ACCESS_VIOLATION, "EXCEPTION_ACCESS_VIOLATION" },
	{ EXCEPTION_READ_FAULT, "EXCEPTION_READ_FAULT" },
	{ EXCEPTION_DATATYPE_MISALIGNMENT, "EXCEPTION_DATATYPE_MISALIGNMENT" },
	{ EXCEPTION_BREAKPOINT, "EXCEPTION_BREAKPOINT" },
	{ EXCEPTION_SINGLE_STEP, "EXCEPTION_SINGLE_STEP" },
	{ EXCEPTION_ARRAY_BOUNDS_EXCEEDED, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED" },
	{ EXCEPTION_FLT_DENORMAL_OPERAND, "EXCEPTION_FLT_DENORMAL_OPERAND" },
	{ EXCEPTION_FLT_DIVIDE_BY_ZERO, "EXCEPTION_FLT_DIVIDE_BY_ZERO" },
	{ EXCEPTION_FLT_INEXACT_RESULT, "EXCEPTION_FLT_INEXACT_RESULT" },
	{ EXCEPTION_FLT_INVALID_OPERATION, "EXCEPTION_FLT_INVALID_OPERATION" },
	{ EXCEPTION_FLT_OVERFLOW, "EXCEPTION_FLT_OVERFLOW" },
	{ EXCEPTION_FLT_STACK_CHECK, "EXCEPTION_FLT_STACK_CHECK" },
	{ EXCEPTION_FLT_UNDERFLOW, "EXCEPTION_FLT_UNDERFLOW" },
	{ EXCEPTION_INT_DIVIDE_BY_ZERO, "EXCEPTION_INT_DIVIDE_BY_ZERO" },
	{ EXCEPTION_INT_OVERFLOW, "EXCEPTION_INT_OVERFLOW" },
	{ EXCEPTION_PRIV_INSTRUCTION, "EXCEPTION_PRIV_INSTRUCTION" },
	{ EXCEPTION_NONCONTINUABLE_EXCEPTION, "EXCEPTION_NONCONTINUABLE_EXCEPTION" },
	{ EXCEPTION_STACK_OVERFLOW, "EXCEPTION_STACK_OVERFLOW"},
	{ EXCEPTION_INVALID_DISPOSITION, "EXCEPTION_INVALID_DISPOSITION" },
	{ EXCEPTION_GUARD_PAGE, "EXCEPTION_GUARD_PAGE" },
	{ EXCEPTION_INVALID_HANDLE, "EXCEPTION_INVALID_HANDLE" }
};
#pragma endregion

void SEHcatch::printStack()
{
	void* stack[MAX_LENGTH];
	const auto process = LF(GetCurrentProcess)();
#ifndef _DEBUG
	LF(SymInitialize).cached()(process, NULL, TRUE);
#else
	SymInitialize(process, NULL, TRUE);
#endif

	const auto frames = LF(CaptureStackBackTrace)(NULL, MAX_FRAMES, stack, nullptr);
	const auto symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
	symbol->MaxNameLen = MAX_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (int i = 3; i < frames; i++) // skip 3 records which are useless to print, you might call some other walkstack functions to get more info
	{
#ifndef _DEBUG
		LF(SymFromAddr).cached()(process, reinterpret_cast<DWORD64>(stack[i]), NULL, symbol);
#else
		SymFromAddr(process, reinterpret_cast<DWORD64>(stack[i]), NULL, symbol);
#endif
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
				const auto symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
				symbol->MaxNameLen = MAX_LENGTH;
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

				std::stringstream ss;

				const auto addr = pExceptionInfo->ExceptionRecord->ExceptionAddress;
				const auto name = symbol->Name;
				const auto flag = pExceptionInfo->ExceptionRecord->ExceptionFlags;
				const auto params = pExceptionInfo->ExceptionRecord->NumberParameters;

				ss << std::format(XOR("Exception (fatal) {} at address {} ({}), flags - {}, params - {}"), crashName, addr, name, flag, params);
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

				console.log(TypeLogs::LOG_ERR, ss.str());
				printStack();
				LF(MessageBoxA)(nullptr, ss.str().c_str(), XOR("Fatal error!"), MB_ICONERROR | MB_OK);
				free(symbol);
				return true;
			} ();

			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}