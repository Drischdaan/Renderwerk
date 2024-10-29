#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Threading/Mutex.h"

DECLARE_LOG_CATEGORY(LogEngine, Trace);

DECLARE_DELEGATE(SignalReceived, uint32);

class RENDERWERK_API FEngine
{
public:
	FEngine();
	~FEngine();

	DELETE_COPY_AND_MOVE(FEngine);

public:
	void RequestExit();

private:
	void Run();

	void Initialize();
	void Shutdown();

	void SignalHandler(int32 Signal);

private:
	static void RegisterInterruptSignals();

private:
	static FSignalReceivedDelegate OnSignalReceived;

private:
	FMutex RunningMutex;
	bool8 bIsRunning = true;

	friend void GuardedMain();
};

/**
 * Global engine pointer. You should use GetEngine() to safely access this pointer.
 * Please check the validity of the pointer before using it.
 */
RENDERWERK_API extern TSharedPtr<FEngine> GEngine;

/**
 * Checks the validity of the global engine pointer and returns it.
 * @return The global engine pointer.
 */
RENDERWERK_API TSharedPtr<FEngine> GetEngine();
