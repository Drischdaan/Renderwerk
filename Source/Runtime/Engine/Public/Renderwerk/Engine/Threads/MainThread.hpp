#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Engine/EngineThread.hpp"

class FWindow;

class ENGINE_API FMainThread : public IEngineThread
{
public:
	FMainThread(TAtomic<bool8>* InShouldRun);
	~FMainThread() override = default;

	DEFAULT_COPY_MOVEABLE(FMainThread)

private:
	void Initialize() override;
	void OnTick() override;
	void Shutdown() override;

private:
	TRef<FWindow> Window;

	friend class ENGINE_API FEngine;
};
