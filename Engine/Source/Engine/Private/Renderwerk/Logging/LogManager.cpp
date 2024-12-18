#include "pch.h"

#include "Renderwerk/Logging/LogManager.h"

#include "Renderwerk/Logging/LogChannel.h"

#include <spdlog/async.h>

#include "spdlog/sinks/stdout_color_sinks.h"

#define LOG_PATTERN "%^[%T.%e] [%t] [%n] %8l:%$ %v"

DEFINE_LOG_CHANNEL(LogDefault);

void FLogManager::RegisterChannel(const ILogChannel& Channel)
{
	if (spdlog::get(Channel.GetName().c_str()) != nullptr)
		return;
	spdlog::sinks_init_list Sinks = {
		std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
	};
	const TSharedPtr<FAsyncLogger> Logger = MakeShared<FAsyncLogger>(Channel.GetName().c_str(), Sinks, spdlog::thread_pool());
	Logger->set_level(ConvertVerbosity(SPDLOG_ACTIVE_LEVEL));
	Logger->set_pattern(LOG_PATTERN);
	register_logger(Logger);
}

TSharedPtr<FLogger> FLogManager::GetLogger(const ILogChannel& Channel)
{
	if (spdlog::get(Channel.GetName().c_str()) == nullptr)
		RegisterChannel(Channel);
	return spdlog::get(Channel.GetName().c_str());
}

ENativeVerbosity FLogManager::ConvertVerbosity(const ELogVerbosity Verbosity)
{
	switch (Verbosity)
	{
	case ELogVerbosity::Fatal: return spdlog::level::critical;
	case ELogVerbosity::Error: return spdlog::level::err;
	case ELogVerbosity::Warning: return spdlog::level::warn;
	case ELogVerbosity::Info: return spdlog::level::info;
	case ELogVerbosity::Debug: return spdlog::level::debug;
	case ELogVerbosity::Trace:
	default:
		return spdlog::level::trace;
	}
}

ENativeVerbosity FLogManager::ConvertVerbosity(const uint32 Verbosity)
{
	switch (Verbosity)
	{
	case SPDLOG_LEVEL_CRITICAL: return spdlog::level::critical;
	case SPDLOG_LEVEL_ERROR: return spdlog::level::err;
	case SPDLOG_LEVEL_WARN: return spdlog::level::warn;
	case SPDLOG_LEVEL_INFO: return spdlog::level::info;
	case SPDLOG_LEVEL_DEBUG: return spdlog::level::debug;
	case SPDLOG_LEVEL_TRACE: return spdlog::level::trace;
	default:
		return spdlog::level::warn;
	}
}

void FLogManager::Initialize()
{
	spdlog::init_thread_pool(8192, 1);
	spdlog::set_level(ConvertVerbosity(SPDLOG_ACTIVE_LEVEL));
	spdlog::set_pattern(LOG_PATTERN);

	RegisterChannel(LogDefault);
	RW_LOG(LogDefault, Info, "LogManager initialized");
}

void FLogManager::Shutdown()
{
	spdlog::shutdown();
}
