#include "pch.h"

#include "Renderwerk/Graphics/GraphicsSwapchain.h"

#include "Renderwerk/Graphics/GraphicsCommandQueue.h"

FGraphicsSwapchain::FGraphicsSwapchain(const TSharedPtr<FGraphicsContext>& InContext)
	: Context(InContext)
{
}

FGraphicsSwapchain::~FGraphicsSwapchain() = default;

void FGraphicsSwapchain::Initialize(const FGraphicsSwapchainDesc& InDescription)
{
	Description = InDescription;

	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	const VkResult Result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context->PhysicalDevice, Description.Surface, &SurfaceCapabilities);
	ASSERT(Result == VK_SUCCESS, "Failed to get surface capabilities");

	if (Description.BufferCount < SurfaceCapabilities.minImageCount)
	{
		RW_LOG(LogGraphics, Warning, "Buffer count is less than the minimum required by the surface. New buffer count: {}", SurfaceCapabilities.minImageCount);
		Description.BufferCount = SurfaceCapabilities.minImageCount;
	}
	else if (Description.BufferCount > SurfaceCapabilities.maxImageCount)
	{
		RW_LOG(LogGraphics, Warning, "Buffer count is greater than the maximum allowed by the surface. New buffer count: {}", SurfaceCapabilities.maxImageCount);
		Description.BufferCount = SurfaceCapabilities.maxImageCount;
	}

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
	PROFILE_FUNCTION();
	ReleaseBackBuffers();
	Swapchain = RecreateSwapchain(Swapchain);
	AcquireBackBuffers();
}

bool8 FGraphicsSwapchain::AcquireNextImageIndex(const VkSemaphore SignalSemaphore)
{
	PROFILE_FUNCTION();
	const VkResult Result = vkAcquireNextImageKHR(Context->Device, Swapchain, UINT64_MAX, SignalSemaphore, VK_NULL_HANDLE, &CurrentImageIndex);
	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		return false;
	ASSERT(Result == VK_SUCCESS, "Failed to acquire next image index");
	return true;
}

bool8 FGraphicsSwapchain::Present(const TSharedPtr<FGraphicsCommandQueue>& CommandQueue, const VkSemaphore WaitSemaphore) const
{
	PROFILE_FUNCTION();
	VkPresentInfoKHR PresentInfo = Vulkan::CreateStructure<VkPresentInfoKHR>();
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &Swapchain;
	PresentInfo.pImageIndices = &CurrentImageIndex;
	PresentInfo.waitSemaphoreCount = WaitSemaphore != VK_NULL_HANDLE;
	PresentInfo.pWaitSemaphores = &WaitSemaphore;

	const VkResult Result = vkQueuePresentKHR(CommandQueue->GetHandle(), &PresentInfo);
	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		return false;
	ASSERT(Result == VK_SUCCESS, "Failed to present");
	return true;
}

FGraphicsBackBuffer FGraphicsSwapchain::GetBackBuffer(const uint32 Index) const
{
	return BackBuffers[Index];
}

FGraphicsBackBuffer FGraphicsSwapchain::GetCurrentBackBuffer() const
{
	return GetBackBuffer(CurrentImageIndex);
}

VkSwapchainKHR FGraphicsSwapchain::RecreateSwapchain(const VkSwapchainKHR OldSwapchain)
{
	PROFILE_FUNCTION();
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	VkResult Result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context->PhysicalDevice, Description.Surface, &SurfaceCapabilities);
	ASSERT(Result == VK_SUCCESS, "Failed to get surface capabilities");

	VkSwapchainCreateInfoKHR SwapchainCreateInfo = Vulkan::CreateStructure<VkSwapchainCreateInfoKHR>();
	SwapchainCreateInfo.surface = Description.Surface;
	SwapchainCreateInfo.minImageCount = Description.BufferCount;
	SwapchainCreateInfo.imageFormat = Description.Format;
	SwapchainCreateInfo.imageColorSpace = Description.ColorSpace;
	SwapchainCreateInfo.imageExtent = SurfaceCapabilities.currentExtent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapchainCreateInfo.preTransform = SurfaceCapabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.presentMode = Description.PresentMode;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = OldSwapchain;

	Extent = SwapchainCreateInfo.imageExtent;

	VkSwapchainKHR NewSwapchain = VK_NULL_HANDLE;
	Result = vkCreateSwapchainKHR(Context->Device, &SwapchainCreateInfo, Context->Allocator, &NewSwapchain);
	ASSERT(Result == VK_SUCCESS, "Failed to create swapchain");
	if (OldSwapchain != VK_NULL_HANDLE)
		DestroySwapchain();
	RW_LOG(LogGraphics, Trace, "Recreated swapchain with {} buffers (Format: {}, PresentMode: {}, Size: {}x{})", Description.BufferCount,
	       GetEnumValueName(Description.Format),
	       GetEnumValueName(Description.PresentMode), Extent.width, Extent.height);
	return NewSwapchain;
}

void FGraphicsSwapchain::DestroySwapchain() const
{
	vkDestroySwapchainKHR(Context->Device, Swapchain, Context->Allocator);
}

void FGraphicsSwapchain::AcquireBackBuffers()
{
	PROFILE_FUNCTION();
	uint32 ImageCount = 0;
	VkResult Result = vkGetSwapchainImagesKHR(Context->Device, Swapchain, &ImageCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to get swapchain images");
	TVector<VkImage> Images(ImageCount);
	Result = vkGetSwapchainImagesKHR(Context->Device, Swapchain, &ImageCount, Images.data());
	ASSERT(Result == VK_SUCCESS, "Failed to get swapchain images");
	BackBuffers.resize(ImageCount);
	for (uint32 Index = 0; Index < ImageCount; ++Index)
	{
		BackBuffers[Index].Image = Images[Index];

		VkImageViewCreateInfo ImageViewInfo = Vulkan::CreateStructure<VkImageViewCreateInfo>();
		ImageViewInfo.image = BackBuffers[Index].Image;
		ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewInfo.format = Description.Format;
		ImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewInfo.subresourceRange.baseMipLevel = 0;
		ImageViewInfo.subresourceRange.levelCount = 1;
		ImageViewInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewInfo.subresourceRange.layerCount = 1;

		Result = vkCreateImageView(Context->Device, &ImageViewInfo, Context->Allocator, &BackBuffers[Index].ImageView);
		ASSERT(Result == VK_SUCCESS, "Failed to create image view");
	}
}

void FGraphicsSwapchain::ReleaseBackBuffers()
{
	for (const FGraphicsBackBuffer& BackBuffer : BackBuffers)
		vkDestroyImageView(Context->Device, BackBuffer.ImageView, Context->Allocator);
	BackBuffers.clear();
}
