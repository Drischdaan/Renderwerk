#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

enum class ENGINE_API ELogVerbosity : uint8
{
	Critical = 0,
	Error,
	Warn,
	Info,
	Debug,
	Trace,
};

ENGINE_API FString ToString(ELogVerbosity Verbosity);
ENGINE_API ELogVerbosity FromString(const FString& Verbosity);
ENGINE_API ELogVerbosity FromInt(uint8 Verbosity);
