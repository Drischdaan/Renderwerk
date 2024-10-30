#pragma once

#include "Renderwerk/Engine/System.h"
#include "Renderwerk/RHI/RHICommon.h"

DECLARE_LOG_CATEGORY(LogRenderer, Trace);

class RENDERWERK_API FRendererSystem : public ISystem
{
public:
	FRendererSystem();
	~FRendererSystem() override;

	DELETE_COPY_AND_MOVE(FRendererSystem);

private:
	TLocalPtr<IRHI> RHI;
};
