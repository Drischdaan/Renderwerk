#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsContext.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"

struct ENGINE_API FGraphicsSwapchainDesc
{
	VkSurfaceKHR Surface;
	uint32 BackBufferCount = 3;
	VkFormat BackBufferFormat = VK_FORMAT_B8G8R8A8_UNORM;
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
	FGraphicsSwapchain(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const TSharedPtr<FGraphicsDevice>& InGraphicsDevice);
	~FGraphicsSwapchain();

	DELETE_COPY_AND_MOVE(FGraphicsSwapchain);

public:
	void Initialize(const FGraphicsSwapchainDesc& InSwapchainDesc);
	void Destroy();

	void Resize();

	[[nodiscard]] bool8 AcquireNextImageIndex(VkSemaphore SignalSemaphore);
	[[nodiscard]] bool8 Present(VkSemaphore WaitSemaphore) const;

	[[nodiscard]] FGraphicsBackBuffer GetBackBuffer(uint32 Index) const;

public:
	[[nodiscard]] uint32 GetCurrentImageIndex() const { return CurrentImageIndex; }

private:
	VkSwapchainKHR RecreateSwapchain(VkSwapchainKHR OldSwapchain = VK_NULL_HANDLE) const;
	void AcquireBackBuffers();
	void ReleaseBackBuffers();
	void DestroySwapchain() const;

private:
	TSharedPtr<FGraphicsContext> GraphicsContext;
	TSharedPtr<FGraphicsDevice> GraphicsDevice;
	FGraphicsSwapchainDesc Description = {};

	VkSwapchainKHR Swapchain = VK_NULL_HANDLE;

	TVector<FGraphicsBackBuffer> BackBuffers;

	uint32 CurrentImageIndex = 0;
};
