#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"
#include "Renderwerk/Graphics/GraphicsCommandBuffer.h"
#include "Renderwerk/Graphics/GraphicsContext.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"
#include "Renderwerk/Graphics/GraphicsSwapchain.h"

class ENGINE_API FGraphicsApi
{
public:
	FGraphicsApi();
	~FGraphicsApi();

	DELETE_COPY_AND_MOVE(FGraphicsApi);

public:
	void Initialize();
	void Destroy() const;

	VkSurfaceKHR CreateSurface(const TSharedPtr<FWindow>& Window) const;
	TSharedPtr<FGraphicsAdapter> ChooseCompatibleAdapter(const VkSurfaceKHR& Surface) const;
	TSharedPtr<FGraphicsDevice> CreateDevice(const TSharedPtr<FGraphicsAdapter>& Adapter) const;
	TSharedPtr<FGraphicsSwapchain> CreateSwapchain(const TSharedPtr<FGraphicsDevice>& Device) const;

	TSharedPtr<FGraphicsCommandBuffer> AllocateCommandBuffer(const TSharedPtr<FGraphicsDevice>& Device, VkCommandPool CommandPool) const;
	void DeallocateCommandBuffer(const TSharedPtr<FGraphicsDevice>& Device, VkCommandPool CommandPool, const TSharedPtr<FGraphicsCommandBuffer>& CommandBuffer) const;

	void SubmitQueue(VkQueue Queue, const FGraphicsFrame& Frame);

	void DestroySurface(const VkSurfaceKHR& Surface) const;

public:
	[[nodiscard]] TSharedPtr<FGraphicsContext> GetGraphicsContext() const { return GraphicsContext; }

private:
	TSharedPtr<FGraphicsContext> GraphicsContext;
};
