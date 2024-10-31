#pragma once

#include "SystemManager.h"

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Threading/Mutex.h"

class ISystem;

DECLARE_LOG_CATEGORY(LogEngine, Trace);

DECLARE_DELEGATE(SignalReceived, uint32);
DECLARE_MULTICAST_DELEGATE(Tick, float64);

class RENDERWERK_API FEngine
{
public:
	FEngine();
	~FEngine();

	DELETE_COPY_AND_MOVE(FEngine);

public:
	void RequestExit();

public:
	NODISCARD TSharedPtr<FSystemManger> GetSystemManager() const { return SystemManager; }

public:
	static FTickDelegate& GetTickDelegate() { return OnTick; }

private:
	void Run();

	void Initialize();
	void Loop() const;
	void Shutdown();

	void SignalHandler(int32 Signal);

private:
	static void RegisterInterruptSignals();

private:
	static FSignalReceivedDelegate OnSignalReceived;
	static FTickDelegate OnTick;

private:
	FMutex RunningMutex;
	bool8 bIsRunning = true;

	TSharedPtr<FSystemManger> SystemManager;

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

/**
 * Convenience function to get a system.
 * @tparam TSystem The system type.
 * @return The system.
 */
template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
RENDERWERK_API INLINE TSharedPtr<TSystem> GetSystem()
{
	return GetEngine()->GetSystemManager()->Get<TSystem>();
}
