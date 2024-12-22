#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

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
	TSharedPtr<FGraphicsAdapter> SelectAdapter(const TSpan<const char*>& RequiredExtensions) const;

private:
	FRendererDesc Description = {};

	TSharedPtr<FGraphicsBackend> GraphicsBackend;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	TSharedPtr<FGraphicsDevice> GraphicsDevice;
};
