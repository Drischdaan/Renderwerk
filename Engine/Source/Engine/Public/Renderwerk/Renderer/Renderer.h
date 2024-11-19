#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Platform/Logging/LogChannel.h"
#include "Renderwerk/Renderer/Adapter.h"

#include <vulkan/vulkan.h>

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
	void CreateInstance();
	void CreateSurface();

	TVector<TSharedPtr<FAdapter>> QueryAdapters() const;
	TSharedPtr<FAdapter> ChooseAdapter(const TVector<TSharedPtr<FAdapter>>& Adapters) const;

private:
	static void CheckExtensions(const TVector<const char*>& Extensions);
	static void CheckLayers(const TVector<const char*>& Layers);

private:
	FRendererDesc Description;

	VkAllocationCallbacks* Allocator = nullptr;

	uint32 ApiVersion;
	VkInstance Instance;

	VkSurfaceKHR Surface;
};
