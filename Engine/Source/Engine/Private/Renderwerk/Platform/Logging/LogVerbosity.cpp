#include "pch.h"

#include "Renderwerk/Platform/Logging/LogVerbosity.h"

FString ToString(const ELogVerbosity Verbosity)
{
	switch (Verbosity)
	{
	DEFINE_ENUM_CASE(ELogVerbosity, Critical);
	DEFINE_ENUM_CASE(ELogVerbosity, Error);
	DEFINE_ENUM_CASE(ELogVerbosity, Warn);
	DEFINE_ENUM_CASE(ELogVerbosity, Info);
	DEFINE_ENUM_CASE(ELogVerbosity, Debug);
	DEFINE_ENUM_CASE(ELogVerbosity, Trace);
	default:
		return "Unknown";
	}
}

ELogVerbosity FromString(const FString& Verbosity)
{
	if (Verbosity == "Critical")
		return ELogVerbosity::Critical;
	if (Verbosity == "Error")
		return ELogVerbosity::Error;
	if (Verbosity == "Warn")
		return ELogVerbosity::Warn;
	if (Verbosity == "Info")
		return ELogVerbosity::Info;
	if (Verbosity == "Debug")
		return ELogVerbosity::Debug;
	if (Verbosity == "Trace")
		return ELogVerbosity::Trace;
	return ELogVerbosity::Critical;
}

ELogVerbosity FromInt(const uint8 Verbosity)
{
	switch (Verbosity)
	{
	case static_cast<int32>(ELogVerbosity::Critical):
		return ELogVerbosity::Critical;
	case static_cast<int32>(ELogVerbosity::Error):
		return ELogVerbosity::Error;
	case static_cast<int32>(ELogVerbosity::Warn):
		return ELogVerbosity::Warn;
	case static_cast<int32>(ELogVerbosity::Info):
		return ELogVerbosity::Info;
	case static_cast<int32>(ELogVerbosity::Debug):
		return ELogVerbosity::Debug;
	case static_cast<int32>(ELogVerbosity::Trace):
		return ELogVerbosity::Trace;
	default:
		return ELogVerbosity::Critical;
	}
}
