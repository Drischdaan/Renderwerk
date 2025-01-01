#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"

DECLARE_LOG_CHANNEL(LogRenderer);

enum : uint8
{
	RENDERER_FRAME_COUNT = 3,
};

struct ENGINE_API FRendererDesc
{
	TSharedPtr<FWindow> Window;
	EGraphicsBackendType BackendType = EGraphicsBackendType::None;
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

	TUniquePtr<IGraphicsBackend> GraphicsBackend;
};
