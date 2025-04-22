#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Engine/EngineThread.hpp"

class ENGINE_API FRenderThread : public IEngineThread
{
public:
	FRenderThread(TAtomic<bool8>* InShouldRun);
	~FRenderThread() override = default;

	DEFAULT_COPY_MOVEABLE(FRenderThread)

private:
	void Initialize() override;
	void OnTick() override;
	void Shutdown() override;
};
