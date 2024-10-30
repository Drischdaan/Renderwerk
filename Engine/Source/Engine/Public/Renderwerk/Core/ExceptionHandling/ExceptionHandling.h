#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/Types/CoreTypes.h"
#include "Renderwerk/Core/Types/String.h"

#include <Windows.h>

struct RENDERWERK_API FExceptionInfo
{
	FString Message;
	FString File;
	uint32 Line = 0;
	FString Function;
	HRESULT CustomCode = 0;

	FExceptionInfo(FString InMessage, FString InFile, const uint32 InLine, FString InFunction, const HRESULT& InCustomCode = 0)
		: Message(std::move(InMessage)), File(std::move(InFile)), Line(InLine), Function(std::move(InFunction)), CustomCode(InCustomCode)
	{
		File = File.find_last_of(TEXT("\\")) != FString::npos ? File.substr(File.find_last_of(TEXT("\\")) + 1) : File;
	}
};

class RENDERWERK_API FExceptionHandling
{
public:
	static LONG Handler(EXCEPTION_POINTERS* ExceptionInfo);

	static void Report(const FExceptionInfo& ExceptionInfo);

	static FString GetResultString(HRESULT Result);

private:
};
