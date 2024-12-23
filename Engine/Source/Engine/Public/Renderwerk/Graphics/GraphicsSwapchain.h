#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FGraphicsSwapchainDesc
{
	VkSurfaceKHR Surface;
	uint32 BufferCount = 3;
	VkFormat Format = VK_FORMAT_B8G8R8A8_UNORM;
	VkColorSpaceKHR ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
};

struct ENGINE_API FGraphicsBackBuffer
{
	VkImage Image = VK_NULL_HANDLE;
	VkImageView ImageView = VK_NULL_HANDLE;
};

class ENGINE_API FGraphicsSwapchain
{
public:
	FGraphicsSwapchain(const TSharedPtr<FGraphicsContext>& InContext);
	~FGraphicsSwapchain();

	DELETE_COPY_AND_MOVE(FGraphicsSwapchain);

public:
	void Initialize(const FGraphicsSwapchainDesc& InDescription);
	void Destroy();

	void Resize();

	[[nodiscard]] bool8 AcquireNextImageIndex(VkSemaphore SignalSemaphore = VK_NULL_HANDLE);
	[[nodiscard]] bool8 Present(const TSharedPtr<FGraphicsCommandQueue>& CommandQueue, VkSemaphore WaitSemaphore = VK_NULL_HANDLE) const;

	[[nodiscard]] FGraphicsBackBuffer GetBackBuffer(uint32 Index) const;
	[[nodiscard]] FGraphicsBackBuffer GetCurrentBackBuffer() const;

public:
	[[nodiscard]] uint32 GetCurrentImageIndex() const { return CurrentImageIndex; }

private:
	VkSwapchainKHR RecreateSwapchain(VkSwapchainKHR OldSwapchain = VK_NULL_HANDLE) const;
	void DestroySwapchain() const;

	void AcquireBackBuffers();
	void ReleaseBackBuffers();

private:
	TSharedPtr<FGraphicsContext> Context;

	FGraphicsSwapchainDesc Description = {};

	VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
	TVector<FGraphicsBackBuffer> BackBuffers;

	uint32 CurrentImageIndex = 0;
};
