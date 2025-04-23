#include "pch.hpp"

#include "Renderwerk/Core/Logging/Logger.hpp"

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"

#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

bool8 FLogger::IsAvailable()
{
	return spdlog::default_logger() != nullptr;
}

void FLogger::Log(ELogVerbosity Verbosity, const FWideString& Message)
{
	if (!IsAvailable())
	{
		return;
	}
	spdlog::log(static_cast<spdlog::level::level_enum>(Verbosity), Message);
}

void FLogger::Log(ELogVerbosity Verbosity, const FWideString& Message, const FLoggerLocation& Location)
{
	if (!IsAvailable())
	{
		return;
	}
	const spdlog::source_loc SourceLocation(Location.file_name(), static_cast<int32>(Location.line()), Location.function_name());
	spdlog::log(SourceLocation, static_cast<spdlog::level::level_enum>(Verbosity), Message);
}

void FLogger::Log(ELogVerbosity Verbosity, const FAnsiString& Message)
{
	if (!IsAvailable())
	{
		return;
	}
	spdlog::log(static_cast<spdlog::level::level_enum>(Verbosity), Message);
}

void FLogger::Log(ELogVerbosity Verbosity, const FAnsiString& Message, const FLoggerLocation& Location)
{
	if (!IsAvailable())
	{
		return;
	}
	const spdlog::source_loc SourceLocation(Location.file_name(), static_cast<int32>(Location.line()), Location.function_name());
	spdlog::log(SourceLocation, static_cast<spdlog::level::level_enum>(Verbosity), Message);
}

void FLogger::Initialize()
{
	spdlog::init_thread_pool(8192, 1);

	TVector<TRef<spdlog::sinks::sink>> Sinks = {
		NewRef<spdlog::sinks::stdout_color_sink_mt>(),
	};
	const TRef<spdlog::async_logger> Logger = NewRef<spdlog::async_logger>("Renderwerk", Sinks.begin(), Sinks.end(), spdlog::thread_pool());
	Logger->set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
	Logger->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%t] [%s:%#] [%l]%$ %v");

	spdlog::register_logger(Logger);
	spdlog::set_default_logger(Logger);
	spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
}

void FLogger::Shutdown()
{
	spdlog::shutdown();
}
