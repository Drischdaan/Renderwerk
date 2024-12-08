#pragma once

#include "Renderwerk/Core/CoreAPI.h"

#include <format>

#define IR_UNKNOWN MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0)
#define IR_VERIFY MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 1)
#define IR_ASSERT MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 2)

[[nodiscard]] ENGINE_API FString GetInterruptionCodeString(HRESULT Code);

struct ENGINE_API FInterruptionContext
{
	FSourceLocation Location;
	FString Reason;

	explicit FInterruptionContext(const FSourceLocation& Location)
		: Location(Location)
	{
	}

	FInterruptionContext(const FString& Reason, const FSourceLocation& Location)
		: Location(Location), Reason(Reason)
	{
	}
};

ENGINE_API void HandleInterruption(HRESULT Code, const FInterruptionContext& Context);
ENGINE_API void HandleFailedVerify(const FInterruptionContext& Context);
ENGINE_API void HandleFailedAssertion(const FInterruptionContext& Context);

ENGINE_API LONG ExceptionHandler(const EXCEPTION_POINTERS* Pointers);

#define INTERRUPT(Code, Message, ...) HandleInterruption(Code, FInterruptionContext(std::format(TEXT(Message), __VA_ARGS__).c_str(), CURRENT_SOURCE_LOCATION))

#define ASSERT(Condition, Message, ...) if (!(Condition)) { HandleFailedAssertion(FInterruptionContext(std::format(TEXT(Message), __VA_ARGS__).c_str(), CURRENT_SOURCE_LOCATION)); }

#if RW_CONFIG_DEBUG || RW_CONFIG_DEVELOPMENT
#	define VERIFY(Condition, Message, ...) if (!(Condition)) { HandleFailedVerify(FInterruptionContext(std::format(TEXT(Message), __VA_ARGS__).c_str(), CURRENT_SOURCE_LOCATION)); }
#else
#	define VERIFY(Condition, Message, ...)
#endif
