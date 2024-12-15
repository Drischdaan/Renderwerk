#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Threading/SyncPoint.h"
#include "Renderwerk/Threading/ThreadTypes.h"

class FWindow;
class FRenderer;

class ENGINE_API IEngineThread
{
public:
	IEngineThread(TAtomic<bool8>* InIsRunning, const FAnsiString& InProfilerName = "UnnamedThread");
	virtual ~IEngineThread();

	DELETE_COPY_AND_MOVE(IEngineThread);

public:
	void Signal();

public:
	[[nodiscard]] ESyncPointState GetSyncPointState() const { return SyncPoint.GetState(); }

protected:
	virtual void Initialize();
	virtual bool8 PreTick();
	virtual bool8 Tick();
	virtual void PostTick();
	virtual void Shutdown();

protected:
	virtual void ThreadEntrypoint();

protected:
	TAtomic<bool8>* bIsRunning;
	FAnsiString ProfilerName;

	FThread Thread;
	FSyncPoint SyncPoint;
};

class ENGINE_API FRenderThread : public IEngineThread
{
public:
	FRenderThread(TAtomic<bool8>* bIsRunning);
	~FRenderThread() override;

	DELETE_COPY_AND_MOVE(FRenderThread);

private:
	void Initialize() override;
	void Shutdown() override;

	bool8 PreTick() override;
	bool8 Tick() override;
	void PostTick() override;

private:
	void ProcessWindowEvents() const;

private:
	TSharedPtr<FWindow> Window;
	TSharedPtr<FRenderer> Renderer;
};

class ENGINE_API FUpdateThread : public IEngineThread
{
public:
	FUpdateThread(TAtomic<bool8>* bIsRunning);
	~FUpdateThread() override;

	DELETE_COPY_AND_MOVE(FUpdateThread);

private:
	bool8 PreTick() override;
	void PostTick() override;

private:
};
