#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/String.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Core/Misc/Atomic.hpp"
#include "Renderwerk/Platform/Threading/ConditionVariable.hpp"
#include "Renderwerk/Platform/Threading/CriticalSection.hpp"

class FThread;

enum class ENGINE_API EEngineThreadState : uint8
{
	Created = 0,
	Initialized,
	Running,
	Exited,
};

class ENGINE_API IEngineThread
{
public:
	IEngineThread(FString InName, TAtomic<bool8>* InShouldRun, bool8 bUseExistingThread = false);
	virtual ~IEngineThread();

	DEFAULT_COPY_MOVEABLE(IEngineThread)

public:
	bool8 WaitForStateChange();
	void WaitForState(EEngineThreadState StateToWait);

public:
	[[nodiscard]] FString GetName() const { return Name; }

	[[nodiscard]] EEngineThreadState GetState() const { return State; }

private:
	virtual void Initialize() = 0;
	virtual void OnTick() = 0;
	virtual void Shutdown() = 0;

protected:
	void ChangeState(EEngineThreadState NewState);

protected:
	FString Name;
	TAtomic<bool8>* bShouldRun;

	TAtomic<EEngineThreadState> State = EEngineThreadState::Created;
	TOwned<FThread> Thread;

	FCriticalSection StateChangeSection;
	FConditionVariable StateChangeVariable;
};
