#include "pch.h"

#include "Renderwerk/Launch/FlowControl.h"

FString GetInterruptionCodeString(const HRESULT Code)
{
	switch (Code)
	{
	case IR_VERIFY: return TEXT("IR_VERIFY");
	case IR_ASSERT: return TEXT("IR_ASSERT");
	case IR_UNKNOWN:
	default:
		return TEXT("IR_UNKNOWN");
	}
}

void HandleInterruption(const HRESULT Code, const FInterruptionContext& Context)
{
	ULONG_PTR Parameters[] = {reinterpret_cast<ULONG_PTR>(&Context)};
	RaiseException(Code, EXCEPTION_NONCONTINUABLE, _countof(Parameters), Parameters);
}

void HandleFailedVerify(const FInterruptionContext& Context)
{
	HandleInterruption(IR_VERIFY, Context);
}

void HandleFailedAssertion(const FInterruptionContext& Context)
{
	HandleInterruption(IR_ASSERT, Context);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
LONG ExceptionHandler(EXCEPTION_POINTERS* Pointers)
{
	const HRESULT ErrorCode = Pointers->ExceptionRecord->ExceptionCode;
	// TODO: Implement a proper error handling system that generates a crash report
	if (ErrorCode == IR_VERIFY || ErrorCode == IR_ASSERT || ErrorCode == IR_UNKNOWN)
	{
		const FInterruptionContext* Context = reinterpret_cast<const FInterruptionContext*>(Pointers->ExceptionRecord->ExceptionInformation[0]);
		const FSourceLocation& Location = Context->Location;
		FString Message = Context->Reason + TEXT("\n\n");
		Message += std::format(TEXT("Code: {} ({:#x})\n"), GetInterruptionCodeString(ErrorCode), ErrorCode).c_str();
		Message += std::format(TEXT("File: {}\n"), Location.GetFormattedFile(), Location.Line, Location.Function).c_str();
		Message += std::format(TEXT("Line: {}\n"), Location.Line).c_str();
		Message += std::format(TEXT("Function: {}"), Location.Function).c_str();
		MessageBox(nullptr, Message.c_str(), TEXT("Interruption"), MB_OK | MB_ICONERROR);
	}
	else
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
