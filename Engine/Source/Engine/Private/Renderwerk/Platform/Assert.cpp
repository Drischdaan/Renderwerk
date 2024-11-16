#include "pch.h"

#include "Renderwerk/Platform/Assert.h"

#include <comdef.h>

#include "Renderwerk/Core/Utils/StringUtils.h"
#include "Renderwerk/Platform/Logging/LogManager.h"

void Assert(const bool8 Expression, FAssertionMetadata Metadata)
{
	if (Expression)
		return;
	ULONG_PTR Arguments[] = {reinterpret_cast<ULONG_PTR>(&Metadata)};
	RaiseException(ERROR_CODE_ASSERT, EXCEPTION_NONCONTINUABLE, _countof(Arguments), Arguments);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
LONG ExceptionHandler(EXCEPTION_POINTERS* ExceptionPointers)
{
	const HRESULT ErrorCode = ExceptionPointers->ExceptionRecord->ExceptionCode;
	if (ErrorCode == ERROR_CODE_ASSERT)
	{
		const FAssertionMetadata* Metadata = reinterpret_cast<FAssertionMetadata*>(ExceptionPointers->ExceptionRecord->ExceptionInformation[0]);
		FString FormattedFile = Metadata->File;
		const size64 LastSlashIndex = Metadata->File.find_last_of("\\");
		if (LastSlashIndex != FString::npos)
			FormattedFile = Metadata->File.substr(LastSlashIndex + 1);
		if (Metadata->CustomMessage != "")
			RW_LOG(LogDefault, Error, "{}", Metadata->CustomMessage);
		RW_LOG(LogDefault, Critical, "Assertion \"{}\" failed in {} on line {} (Function: {})", Metadata->Expression, FormattedFile, Metadata->Line,
		       Metadata->Function);
		RW_LOG(LogDefault, Critical, "Code: {}", GetResultString(Metadata->ErrorCode));

		FString MessageBoxMessage = "Assertion failure\n\n";
		if (Metadata->CustomMessage != "")
			MessageBoxMessage += std::format("{}\n\n", Metadata->CustomMessage);
		MessageBoxMessage += std::format("Expression: {}\n", Metadata->Expression);
		MessageBoxMessage += std::format("File: {}\n", FormattedFile);
		MessageBoxMessage += std::format("Line: {}\n", Metadata->Line);
		MessageBoxMessage += std::format("Function: {}\n", Metadata->Function);
		MessageBoxMessage += std::format("Code: {}", GetResultString(Metadata->ErrorCode));
		MessageBoxA(nullptr, MessageBoxMessage.c_str(), "Assertion failure", MB_ICONERROR | MB_OK);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

FString GetResultString(const HRESULT Result)
{
	FString ResultString = "Unknown result";
	const DWORD Facility = HRESULT_FACILITY(Result);
	if (Facility == FACILITY_ITF)
	{
		if (Result == ERROR_CODE_ASSERT)
			ResultString = "Assertion failed";
	}
	else
	{
		const _com_error Error(Result);
		ResultString = Error.ErrorMessage();
	}
	return ResultString;
}
