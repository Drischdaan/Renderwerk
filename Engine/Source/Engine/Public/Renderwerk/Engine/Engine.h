#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Platform/Threading/SyncPoint.h"
#include "Renderwerk/Platform/Threading/ThreadTypes.h"

class ENGINE_API FEngine
{
private:
	struct FEngineThread
	{
		FThread Thread;
		FSyncPoint SyncPoint;
	};

public:
	FEngine() = default;
	~FEngine() = default;

	DELETE_COPY_AND_MOVE(FEngine);

public:
	void RequestExit();

private:
	void Initialize();
	void RunLoop();
	void Shutdown();

private:
	void MainThread_Tick();

	void UpdateThread_Main();
	void UpdateThread_Initialize();
	void UpdateThread_Tick();
	void UpdateThread_Shutdown();

	void RenderThread_Main();
	void RenderThread_Initialize();
	void RenderThread_HandleEvents() const;
	void RenderThread_Tick();
	void RenderThread_Shutdown();

private:
	TSharedPtr<FWindow> Window = nullptr;

	TAtomic<bool8> bStopThreads = false;
	FEngineThread UpdateThread = {};
	FEngineThread RenderThread = {};

	friend void RunEngine();
};

/**
 * Global engine pointer.
 * @note You should use GetEngine() instead of this global pointer or check if it is valid yourself
 */
ENGINE_API extern TSharedPtr<FEngine> GEngine;

/**
 * Get the global engine pointer. This function will assert if the global engine pointer is not valid.
 * @return The global engine pointer
 */
ENGINE_API TSharedPtr<FEngine> GetEngine();
