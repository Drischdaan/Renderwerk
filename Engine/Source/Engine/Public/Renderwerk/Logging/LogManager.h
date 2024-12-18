#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Logging/LogChannel.h"

#include <spdlog/async_logger.h>
#include <spdlog/spdlog.h>

#include "LogVerbosity.h"

DECLARE_LOG_CHANNEL(LogDefault);

using FLogger = spdlog::logger;
using FAsyncLogger = spdlog::async_logger;
using ENativeVerbosity = spdlog::level::level_enum;

class ENGINE_API FLogManager
{
public:
	static void RegisterChannel(const ILogChannel& Channel);
	static TSharedPtr<FLogger> GetLogger(const ILogChannel& Channel);

	static ENativeVerbosity ConvertVerbosity(ELogVerbosity Verbosity);
	static ENativeVerbosity ConvertVerbosity(uint32 Verbosity);

private:
	static void Initialize();
	static void Shutdown();

	friend ENGINE_API int32 LaunchEngine(const FLaunchParameters& Parameters);
};

#define RW_LOG(Category, Verbosity, Format, ...) \
	SPDLOG_LOGGER_CALL(FLogManager::GetLogger(Category), FLogManager::ConvertVerbosity(ELogVerbosity::Verbosity), TEXT(Format), __VA_ARGS__)
