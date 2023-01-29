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

LONG WINAPI SEHcatch::memErrorCatch(EXCEPTION_POINTERS* pExceptionInfo)
{
	DWORD64 displacement = 0;
	char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = MAX_SYM_NAME;

	HANDLE process = GetCurrentProcess();

	if (SymFromAddr(process, (DWORD64)pExceptionInfo->ExceptionRecord->ExceptionAddress, &displacement, symbol))
	{
		const uint32_t maxFrames = 62;
		void* stack[maxFrames];
		const auto frames = CaptureStackBackTrace(0, maxFrames, stack, nullptr);
		SymInitialize(process, NULL, TRUE);

		CONTEXT context = *pExceptionInfo->ContextRecord;
		STACKFRAME64 stackFrame;
		memset(&stackFrame, 0, sizeof(stackFrame));
		DWORD machineType = IMAGE_FILE_MACHINE_I386;
#ifdef _WIN64
		machineType = IMAGE_FILE_MACHINE_AMD64;
		stackFrame.AddrPC.Offset = context.Rip;
		stackFrame.AddrFrame.Offset = context.Rbp;
		stackFrame.AddrStack.Offset = context.Rsp;
#else
		stackFrame.AddrPC.Offset = context.Eip;
		stackFrame.AddrFrame.Offset = context.Ebp;
		stackFrame.AddrStack.Offset = context.Esp;
#endif
		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Mode = AddrModeFlat;
		stackFrame.AddrStack.Mode = AddrModeFlat;

		for (int i = 0; i < frames; i++) 
		{
			if (!StackWalk64(machineType, process, GetCurrentThread(),
				&stackFrame, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
				break;

			console::error("Stack frame {} : 0x{:X}", i, stackFrame.AddrPC.Offset);
		}

		const auto addr = pExceptionInfo->ExceptionRecord->ExceptionAddress;
		const auto name = symbol->Name;
		const auto flag = pExceptionInfo->ExceptionRecord->ExceptionFlags;
		const auto params = pExceptionInfo->ExceptionRecord->NumberParameters;
		const auto crashName = crashNames.at(pExceptionInfo->ExceptionRecord->ExceptionCode);
		const auto info = std::format("Exception (fatal) {} at address {} ({}), flags - {}, params - {}", crashName, addr, name, flag, params);
		console::error("EAX - 0x{:X}", pExceptionInfo->ContextRecord->Eax);
		console::error("EBP - 0x{:X}", pExceptionInfo->ContextRecord->Ebp);
		console::error("EBX - 0x{:X}", pExceptionInfo->ContextRecord->Ebx);
		console::error("ECX - 0x{:X}", pExceptionInfo->ContextRecord->Ecx);
		console::error("EDI - 0x{:X}", pExceptionInfo->ContextRecord->Edi);
		console::error("EDX - 0x{:X}", pExceptionInfo->ContextRecord->Edx);
		console::error("EIP - 0x{:X}", pExceptionInfo->ContextRecord->Eip);
		console::error("ESI - 0x{:X}", pExceptionInfo->ContextRecord->Esi);
		console::error("ESP - 0x{:X}", pExceptionInfo->ContextRecord->Esp);
	}
	else
	{
		console::error("Can't get symbol info about this crash");
	}

	SymCleanup(GetCurrentProcess());


	return EXCEPTION_CONTINUE_EXECUTION;
}