#pragma once

#include <source_location>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/String.hpp"
#include "Renderwerk/Core/Logging/LogVerbosity.hpp"

using FLoggerLocation = std::source_location;

class ENGINE_API FLogger
{
public:
	[[nodiscard]] static bool8 IsAvailable();

	static void Log(ELogVerbosity Verbosity, const FString& Message);
	static void Log(ELogVerbosity Verbosity, const FString& Message, const FLoggerLocation& Location);

private:
	static void Initialize();
	static void Shutdown();

private:
	friend ENGINE_API int32 Launch();
};

#define RW_LOG(Verbosity, Message, ...) \
	do \
	{ \
		if constexpr (static_cast<uint8>(ELogVerbosity::Verbosity) >= static_cast<uint8>(RW_LOG_VERBOSITY)) \
		{ \
			FLogger::Log(ELogVerbosity::Verbosity, FStringUtilities::Format(TEXT(Message), __VA_ARGS__), FLoggerLocation::current()); \
		} \
	} while(false)
