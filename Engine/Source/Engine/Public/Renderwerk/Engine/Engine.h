#pragma once

#include "ArgumentParser.h"
#include "EngineThreads.h"

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Threading/ThreadTypes.h"

class FWindow;
class FWindowManager;
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

	[[nodiscard]] TSharedPtr<FWindowManager> GetWindowManager() const { return WindowManager; }

private:
	FArgumentParser ArgumentParser;

	TAtomic<bool8> bIsRunning = true;

	TSharedPtr<FWindowManager> WindowManager;
	FGuid MainWindowGuid = INVALID_GUID_ID;

	TSharedPtr<FRenderThread> RenderThread;
	TSharedPtr<FUpdateThread> UpdateThread;

	friend void GuardedMain(const struct FLaunchParameters& Parameters);
	friend class ENGINE_API FRenderThread;
	friend class ENGINE_API FUpdateThread;
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
