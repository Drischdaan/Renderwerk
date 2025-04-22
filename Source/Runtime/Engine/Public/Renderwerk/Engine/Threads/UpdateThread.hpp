#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Engine/EngineThread.hpp"

class ENGINE_API FUpdateThread : public IEngineThread
{
public:
	FUpdateThread(TAtomic<bool8>* InShouldRun);
	~FUpdateThread() override;

	DEFAULT_COPY_MOVEABLE(FUpdateThread)

private:
	void Initialize() override;
	void OnTick() override;
	void Shutdown() override;

public:
};
