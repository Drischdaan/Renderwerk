#include "pch.h"

#include "Renderwerk/Platform/Logging/LogManager.h"

#include "spdlog/async.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

DEFINE_LOG_CHANNEL(LogDefault);

#define LOG_PATTERN "%^[%T.%e] [%t] [%n] %8l:%$ %v"

void FLogManager::Initialize()
{
	spdlog::init_thread_pool(8192, 1);

	spdlog::set_level(spdlog::level::trace);
	spdlog::set_pattern(LOG_PATTERN);

	set_default_logger(Register(LogDefault));
	RW_LOG(LogDefault, Info, "{} {} (by {})", RW_ENGINE_NAME, RW_ENGINE_FULL_VERSION, RW_ENGINE_AUTHOR);
	RW_LOG(LogDefault, Info, "LogManager initialized");
}

void FLogManager::Shutdown()
{
	RW_LOG(LogDefault, Info, "LogManager shutting down...");
	spdlog::shutdown();
}

FLoggerHandle FLogManager::Register(const ILogChannel& Channel)
{
	spdlog::sinks_init_list SinksList = {
		std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
		std::make_shared<spdlog::sinks::rotating_file_sink_mt>("Logs/Engine.log", 1048576 * 50, 0, true),
	};
	const std::shared_ptr<spdlog::async_logger> Logger = std::make_shared<spdlog::async_logger>(Channel.GetName(), SinksList, spdlog::thread_pool());
	Logger->set_pattern(LOG_PATTERN);
	register_logger(Logger);
	return Logger;
}

bool8 FLogManager::IsRegistered(const ILogChannel& Channel)
{
	return spdlog::get(Channel.GetName()) != nullptr;
}

FLoggerHandle FLogManager::GetLogger(const ILogChannel& Channel)
{
	if (!IsRegistered(Channel.GetName()))
		return GetDefaultLogger();
	return spdlog::get(Channel.GetName());
}

FLoggerHandle FLogManager::GetDefaultLogger()
{
	return spdlog::default_logger();
}

spdlog::level::level_enum FLogManager::ConvertToNativeVerbosity(const ELogVerbosity Level)
{
	switch (Level)
	{
	case ELogVerbosity::Critical: return spdlog::level::critical;
	case ELogVerbosity::Error: return spdlog::level::err;
	case ELogVerbosity::Warn: return spdlog::level::warn;
	case ELogVerbosity::Info: return spdlog::level::info;
	case ELogVerbosity::Debug: return spdlog::level::debug;
	case ELogVerbosity::Trace: return spdlog::level::trace;
	default:
		return spdlog::level::off;
	}
}
