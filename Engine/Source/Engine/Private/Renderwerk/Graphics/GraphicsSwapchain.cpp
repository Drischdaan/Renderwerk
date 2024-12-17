#include "pch.h"

#include "Renderwerk/Graphics/GraphicsSwapchain.h"

FGraphicsSwapchain::FGraphicsSwapchain(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const TSharedPtr<FGraphicsDevice>& InGraphicsDevice)
	: GraphicsContext(InGraphicsContext), GraphicsDevice(InGraphicsDevice)
{
}

FGraphicsSwapchain::~FGraphicsSwapchain() = default;

void FGraphicsSwapchain::Initialize(const FGraphicsSwapchainDesc& InSwapchainDesc)
{
	Description = InSwapchainDesc;
	Swapchain = RecreateSwapchain();
	AcquireBackBuffers();
}

void FGraphicsSwapchain::Destroy()
{
	ReleaseBackBuffers();
	DestroySwapchain();
}

void FGraphicsSwapchain::Resize()
{
	ReleaseBackBuffers();
	DestroySwapchain();
	Swapchain = RecreateSwapchain(Swapchain);
	AcquireBackBuffers();
}

bool8 FGraphicsSwapchain::AcquireNextImageIndex(const VkSemaphore SignalSemaphore)
{
	const FVulkanResult Result = vkAcquireNextImageKHR(GraphicsDevice->GetHandle(), Swapchain, UINT64_MAX, SignalSemaphore, VK_NULL_HANDLE, &CurrentImageIndex);
	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		return false;
	VERIFY(Result == VK_SUCCESS, "Failed to acquire image index");
	return true;
}

bool8 FGraphicsSwapchain::Present(const VkSemaphore WaitSemaphore) const
{
	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = nullptr;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = &WaitSemaphore;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &Swapchain;
	PresentInfo.pImageIndices = &CurrentImageIndex;
	const FVulkanResult Result = vkQueuePresentKHR(GraphicsDevice->GetPresentQueue(), &PresentInfo);
	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		return false;
	VERIFY(Result == VK_SUCCESS, "Failed to present image");
	return true;
}

FGraphicsBackBuffer FGraphicsSwapchain::GetBackBuffer(const uint32 Index) const
{
	ASSERT(Index < BackBuffers.size(), "Index out of range");
	return BackBuffers.at(Index);
}

VkSwapchainKHR FGraphicsSwapchain::RecreateSwapchain(const VkSwapchainKHR OldSwapchain) const
{
	const FGraphicsSurfaceProperties SurfaceProperties = GraphicsDevice->GetAdapter()->GetSurfaceProperties(Description.Surface);

	VkSwapchainCreateInfoKHR SwapchainCreateInfo;
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.pNext = nullptr;
	SwapchainCreateInfo.flags = 0;
	SwapchainCreateInfo.surface = Description.Surface;
	SwapchainCreateInfo.minImageCount = Description.BackBufferCount;
	SwapchainCreateInfo.imageFormat = Description.BackBufferFormat;
	SwapchainCreateInfo.imageColorSpace = Description.ColorSpace;
	SwapchainCreateInfo.imageExtent = SurfaceProperties.Capabilities.currentExtent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapchainCreateInfo.preTransform = SurfaceProperties.Capabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.queueFamilyIndexCount = 0;
	SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
	SwapchainCreateInfo.presentMode = Description.PresentMode;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = OldSwapchain;

	VkSwapchainKHR NewSwapchain = VK_NULL_HANDLE;
	const FVulkanResult Result = vkCreateSwapchainKHR(GraphicsDevice->GetHandle(), &SwapchainCreateInfo, GraphicsContext->GetAllocator(), &NewSwapchain);
	ASSERT(Result == VK_SUCCESS, "Failed to create swapchain");
	return NewSwapchain;
}

void FGraphicsSwapchain::AcquireBackBuffers()
{
	uint32 BackBufferCount = 0;
	FVulkanResult Result = vkGetSwapchainImagesKHR(GraphicsDevice->GetHandle(), Swapchain, &BackBufferCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to get swapchain images");
	TVector<VkImage> SwapchainImages(BackBufferCount);
	Result = vkGetSwapchainImagesKHR(GraphicsDevice->GetHandle(), Swapchain, &BackBufferCount, SwapchainImages.data());
	ASSERT(Result == VK_SUCCESS, "Failed to get swapchain images");
	BackBuffers.resize(BackBufferCount);
	for (uint32 Index = 0; Index < BackBufferCount; ++Index)
	{
		BackBuffers[Index].Image = SwapchainImages[Index];

		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.image = BackBuffers[Index].Image;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.format = Description.BackBufferFormat;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;

		Result = vkCreateImageView(GraphicsDevice->GetHandle(), &ImageViewCreateInfo, GraphicsContext->GetAllocator(), &BackBuffers[Index].ImageView);
		ASSERT(Result == VK_SUCCESS, "Failed to create image view");
	}
}

void FGraphicsSwapchain::ReleaseBackBuffers()
{
	// For some weird reason, the BackBuffers vector is empty when this function is called.
	// This only happens sometimes, and I have no idea why.
	for (const FGraphicsBackBuffer& BackBufferView : BackBuffers)
		vkDestroyImageView(GraphicsDevice->GetHandle(), BackBufferView.ImageView, GraphicsContext->GetAllocator());
	BackBuffers.clear();
}

void FGraphicsSwapchain::DestroySwapchain() const
{
	vkDestroySwapchainKHR(GraphicsDevice->GetHandle(), Swapchain, GraphicsContext->GetAllocator());
}
