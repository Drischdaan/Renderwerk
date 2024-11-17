#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Logging/LogChannel.h"
#include "Renderwerk/Platform/Logging/LogVerbosity.h"

#include "spdlog/spdlog.h"

#include <memory>

/**
 * Default log channel, if no channel is specified or the specified channel is not registered.
 */
DECLARE_LOG_CHANNEL(LogDefault);

using FLoggerHandle = std::shared_ptr<spdlog::logger>;

class ENGINE_API FLogManager
{
private:
	static void Initialize();
	static void Shutdown();

public:
	/**
	 * Registers a new log channel.
	 * @param Channel The log channel to register.
	 * @return The logger handle.
	 */
	static FLoggerHandle Register(const ILogChannel& Channel);

	/**
	 * Checks if a log channel is registered.
	 * @param Channel The log channel to check.
	 * @return True if the log channel is registered, false otherwise.
	 */
	[[nodiscard]] static bool8 IsRegistered(const ILogChannel& Channel);

	/**
	 * Gets the logger handle for a log channel.
	 * @param Channel The log channel to get the logger handle for.
	 * @return The logger handle.
	 */
	[[nodiscard]] static FLoggerHandle GetLogger(const ILogChannel& Channel);

	/**
	 * Gets the default logger handle.
	 * @return The default logger handle.
	 */
	[[nodiscard]] static FLoggerHandle GetDefaultLogger();

	/**
	 * Converts a log verbosity to the native verbosity.
	 * @param Level The log verbosity to convert.
	 * @return The native verbosity.
	 */
	[[nodiscard]] static spdlog::level::level_enum ConvertToNativeVerbosity(ELogVerbosity Level);

private:
	friend void StartCoreServices();
	friend void ShutdownCoreServices();
};

#define RW_LOG(Channel, Verbosity, Format, ...) \
	SPDLOG_LOGGER_CALL(FLogManager::GetLogger(Channel), FLogManager::ConvertToNativeVerbosity(ELogVerbosity::Verbosity), Format, __VA_ARGS__)
