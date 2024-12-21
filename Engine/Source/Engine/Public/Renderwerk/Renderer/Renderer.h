#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

class FGraphicsBackend;

DECLARE_LOG_CHANNEL(LogRenderer);

struct ENGINE_API FRendererDesc
{
	TSharedPtr<FWindow> Window;
};

class ENGINE_API FRenderer
{
public:
	FRenderer();
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer);

public:
	void Initialize(const FRendererDesc& InDescription);
	void Destroy();

	void Resize();

	void BeginFrame();
	void EndFrame();

private:
	FRendererDesc Description = {};

	TSharedPtr<FGraphicsBackend> GraphicsBackend;
};
