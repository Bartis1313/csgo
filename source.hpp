#pragma once
#include <DbgHelp.h>
#include <WIndows.h>
#include <map>
#include "utilities/utilities.hpp"

// 0x{:X} used because there is no actual syntax for 0xF450 printing for example, it's either like 0xf450 or 0XF450

#define TRACE_MAX_STACK_FRAMES 1024
#define TRACE_MAX_FUNCTION_NAME_LENGTH 1024
//#define SYSTEM_FRAMES_TO_SKIP 4

void printStack()
{
	void* stack[TRACE_MAX_FUNCTION_NAME_LENGTH];
	const auto process = LF(GetCurrentProcess)();
#ifndef _DEBUG
	LF(SymInitialize).cached()(process, NULL, TRUE);
#else
	SymInitialize(process, NULL, TRUE);
#endif

	const auto frames = CaptureStackBackTrace(0, TRACE_MAX_STACK_FRAMES, stack, nullptr);
	const auto symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(TCHAR));
	symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (int i = 1; i < frames; i++)
	{
#ifndef _DEBUG
		LF(SymFromAddr).cached()(process, (DWORD64)(stack[i]), 0, symbol);
#else
		SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
#endif
		LOG(LOG_ERR, std::format(XOR("{}: {} - 0x{:X}"), frames - i - 1, symbol->Name, symbol->Address));
	}

	free(symbol);
}

std::map<DWORD, const char*> crashNames
{
	{EXCEPTION_ACCESS_VIOLATION, XOR("EXCEPTION_ACCESS_VIOLATION")},
	{EXCEPTION_READ_FAULT, XOR("EXCEPTION_READ_FAULT") },
	{EXCEPTION_DATATYPE_MISALIGNMENT, XOR("EXCEPTION_DATATYPE_MISALIGNMENT")},
	{EXCEPTION_BREAKPOINT, XOR("EXCEPTION_BREAKPOINT")},
	{EXCEPTION_SINGLE_STEP, XOR("EXCEPTION_SINGLE_STEP")},
	{EXCEPTION_ARRAY_BOUNDS_EXCEEDED, XOR("EXCEPTION_ARRAY_BOUNDS_EXCEEDED")},
	{EXCEPTION_FLT_DENORMAL_OPERAND, XOR("EXCEPTION_FLT_DENORMAL_OPERAND")},
	{EXCEPTION_FLT_DIVIDE_BY_ZERO, XOR("EXCEPTION_FLT_DIVIDE_BY_ZERO")},
	{EXCEPTION_FLT_INEXACT_RESULT, XOR("EXCEPTION_FLT_INEXACT_RESULT")},
	{EXCEPTION_FLT_INVALID_OPERATION, XOR("EXCEPTION_FLT_INVALID_OPERATION")},
	{EXCEPTION_FLT_OVERFLOW, XOR("EXCEPTION_FLT_OVERFLOW")},
	{EXCEPTION_FLT_STACK_CHECK, XOR("EXCEPTION_FLT_STACK_CHECK")},
	{EXCEPTION_FLT_UNDERFLOW, XOR("EXCEPTION_FLT_UNDERFLOW")},
	{EXCEPTION_INT_DIVIDE_BY_ZERO, XOR("EXCEPTION_INT_DIVIDE_BY_ZERO")},
	{EXCEPTION_INT_OVERFLOW, XOR("EXCEPTION_INT_OVERFLOW")},
	{EXCEPTION_PRIV_INSTRUCTION, XOR("EXCEPTION_PRIV_INSTRUCTION")},
	{EXCEPTION_NONCONTINUABLE_EXCEPTION, XOR("EXCEPTION_NONCONTINUABLE_EXCEPTION")},
	{EXCEPTION_STACK_OVERFLOW, XOR("EXCEPTION_STACK_OVERFLOW")},
	{EXCEPTION_INVALID_DISPOSITION, XOR("EXCEPTION_INVALID_DISPOSITION")},
	{EXCEPTION_GUARD_PAGE, XOR("EXCEPTION_GUARD_PAGE")},
	{EXCEPTION_INVALID_HANDLE, XOR("EXCEPTION_INVALID_HANDLE")}
};

LONG WINAPI memErrorCatch(EXCEPTION_POINTERS* pExceptionInfo)
{
	const auto code = pExceptionInfo->ExceptionRecord->ExceptionCode;

	for (const auto& [number, name] : crashNames)
	{
		if (code == number)
		{
			static bool bOnce = [=]()
			{
				const auto symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
				symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH; // sometimes the name is huge to get it valid
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

				std::stringstream ss;

				const auto addr = pExceptionInfo->ExceptionRecord->ExceptionAddress;
				const auto name = symbol->Name;
				const auto flag = pExceptionInfo->ExceptionRecord->ExceptionFlags;
				const auto params = pExceptionInfo->ExceptionRecord->NumberParameters;

				ss << std::format(XOR("Exception (fatal) {} at address {} ({}), flags - {}, params - {}\n"), crashNames[code], addr, name, flag, params);
				// x86
				LOG(LOG_ERR, std::format(XOR("EAX - 0x{:X}"), pExceptionInfo->ContextRecord->Eax));
				LOG(LOG_ERR, std::format(XOR("EBP - 0x{:X}"), pExceptionInfo->ContextRecord->Ebp));
				LOG(LOG_ERR, std::format(XOR("EBX - 0x{:X}"), pExceptionInfo->ContextRecord->Ebx));
				LOG(LOG_ERR, std::format(XOR("ECX - 0x{:X}"), pExceptionInfo->ContextRecord->Ecx));
				LOG(LOG_ERR, std::format(XOR("EDI - 0x{:X}"), pExceptionInfo->ContextRecord->Edi));
				LOG(LOG_ERR, std::format(XOR("EDX - 0x{:X}"), pExceptionInfo->ContextRecord->Edx));
				LOG(LOG_ERR, std::format(XOR("EIP - 0x{:X}"), pExceptionInfo->ContextRecord->Eip));
				LOG(LOG_ERR, std::format(XOR("ESI - 0x{:X}"), pExceptionInfo->ContextRecord->Esi));
				LOG(LOG_ERR, std::format(XOR("ESP - 0x{:X}"), pExceptionInfo->ContextRecord->Esp));

				LOG(LOG_ERR, ss.str());
				printStack();
				LF(MessageBoxA)(nullptr, ss.str().c_str(), XOR("Fatal error!"), MB_ICONERROR | MB_OK);
				free(symbol);
				ss.clear();

				return true;
			} ();

			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}