#pragma once

#include "ArgumentParser.h"

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/Memory/SmartPointers.h"

/**
 * Main component that glues everything together
 */
class ENGINE_API FEngine
{
public:
	FEngine(TVector<FString>&& InArguments);
	~FEngine();

	DELETE_COPY_AND_MOVE(FEngine);

private:
	/**
	 * Manages the flow of the engines lifecycle (Initialize, RunLoop, Shutdown)
	 * @note Should be called by the entrypoint
	 */
	void Run();

	void Initialize();
	void RunLoop();
	void Shutdown();

public:
	void RequestShutdown();

public:
	[[nodiscard]] FArgumentParser& GetArgumentParser() { return ArgumentParser; }

private:
	FArgumentParser ArgumentParser;

	bool8 bIsRunning = true;

	friend void GuardedMain(const struct FLaunchParameters& Parameters);
};

/**
 * Global engine instance pointer
 * @note Should be checked for validity before usage
 */
ENGINE_API extern TSharedPtr<FEngine> GEngine;

/**
 * Gets the global engine instance
 * @note This automatically checks for validity
 * @return The global engine instance
 */
[[nodiscard]] ENGINE_API TSharedPtr<FEngine> GetEngine();
