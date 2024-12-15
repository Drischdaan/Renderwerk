#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/VulkanContext.h"

struct ENGINE_API FVulkanSwapchainDesc
{
	uint32 BackBufferCount = 3;
	VkFormat BackBufferFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkColorSpaceKHR ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
};

class ENGINE_API FVulkanGraphicsSwapchain
{
public:
	FVulkanGraphicsSwapchain(const FVulkanContext& InContext, const FVulkanSwapchainDesc& InDescription);
	~FVulkanGraphicsSwapchain();

	DELETE_COPY_AND_MOVE(FVulkanGraphicsSwapchain);

public:
	void Resize();

	bool8 AcquireImageIndex(VkSemaphore SignalSemaphore);
	bool8 Present(VkSemaphore WaitSemaphore) const;

	[[nodiscard]] VkImage GetBackBuffer(uint32 Index) const;
	[[nodiscard]] VkImageView GetBackBufferView(uint32 Index) const;

public:
	[[nodiscard]] uint32 GetCurrentImageIndex() const { return CurrentImageIndex; }

private:
	void CreateSwapchain();
	void DestroySwapchain() const;

	void AcquireBackBuffers();
	void ReleaseBackBuffers() const;

private:
	FVulkanContext Context;
	FVulkanSwapchainDesc Description;

	VkSwapchainKHR Swapchain = VK_NULL_HANDLE;

	TVector<VkImage> BackBuffers;
	TVector<VkImageView> BackBufferViews;
	VkExtent2D Extent;

	uint32 CurrentImageIndex = 0;
};
