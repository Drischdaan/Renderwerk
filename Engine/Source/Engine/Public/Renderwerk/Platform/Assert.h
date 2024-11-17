#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include <format>
#include <Windows.h>

#define ERROR_CODE_ASSERT MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0001)

struct ENGINE_API FAssertionMetadata
{
	FString Expression;
	FString File;
	FString Function;
	uint32 Line;
	FString CustomMessage;
	HRESULT ErrorCode = E_OUTOFMEMORY;
};

ENGINE_API void Assert(bool8 Expression, FAssertionMetadata Metadata);

ENGINE_API LONG ExceptionHandler(EXCEPTION_POINTERS* ExceptionPointers);
ENGINE_API FString GetResultString(HRESULT Result);

#define ASSERT(Expression) Assert(Expression, {#Expression, __FILE__, __FUNCSIG__, __LINE__, ""})
#define ASSERT_CODE(Expression, Code) Assert(Expression, {#Expression, __FILE__, __FUNCSIG__, __LINE__, "", Code})
#define ASSERT_MSG(Expression, Format, ...) Assert(Expression, {#Expression, __FILE__, __FUNCSIG__, __LINE__, std::format(Format, __VA_ARGS__)})
#define ASSERT_MSG_CODE(Expression, Code, Format, ...) Assert(Expression, {#Expression, __FILE__, __FUNCSIG__, __LINE__, std::format(Format, __VA_ARGS__), Code})

#if RW_CONFIG_DEBUG || RW_CONFIG_DEVELOPMENT
#	define DEBUG_ASSERT(Expression) ASSERT(Expression)
#	define DEBUG_ASSERT_CODE(Expression, Code) ASSERT_CODE(Expression, Code)
#	define DEBUG_ASSERT_MSG(Expression, Format, ...) ASSERT_MSG(Expression, Format, __VA_ARGS__)
#	define DEBUG_ASSERT_MSG_CODE(Expression, Code, Format, ...) ASSERT_MSG(Expression, Code, Format, __VA_ARGS__)
#else
#	define DEBUG_ASSERT(Expression)
#	define DEBUG_ASSERT_CODE(Expression, Code)
#	define DEBUG_ASSERT_MSG(Expression, Format, ...)
#	define DEBUG_ASSERT_MSG_CODE(Expression, Code, Format, ...)
#endif
