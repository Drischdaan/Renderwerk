#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsFwd.h"

DECLARE_LOG_CHANNEL(LogRenderer);

struct ENGINE_API FRendererDesc
{
	TSharedPtr<FWindow> Window;
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererDesc& InDescription);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer);

private:
	FRendererDesc Description;

	TUniquePtr<FVulkanGraphicsApi> GraphicsApi;
};
