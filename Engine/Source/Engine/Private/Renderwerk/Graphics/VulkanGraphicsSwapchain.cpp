#include "pch.h"

#include "Renderwerk/Graphics/VulkanGraphicsSwapchain.h"

#include "Renderwerk/Graphics/VulkanContext.h"
#include "Renderwerk/Graphics/VulkanGraphicsAdapter.h"
#include "Renderwerk/Graphics/VulkanGraphicsDevice.h"

FVulkanGraphicsSwapchain::FVulkanGraphicsSwapchain(const FVulkanContext& InContext, const FVulkanSwapchainDesc& InDescription)
	: Context(InContext), Description(InDescription)
{
	CreateSwapchain();
	AcquireBackBuffers();
}

FVulkanGraphicsSwapchain::~FVulkanGraphicsSwapchain()
{
	ReleaseBackBuffers();
	DestroySwapchain();
}

void FVulkanGraphicsSwapchain::Resize()
{
	ReleaseBackBuffers();
	DestroySwapchain();
	CreateSwapchain();
	AcquireBackBuffers();
}

void FVulkanGraphicsSwapchain::CreateSwapchain()
{
	const FVulkanSurfaceProperties SurfaceProperties = Context.GraphicsDevice->GetAdapter()->GetSurfaceProperties(Context.Surface);

	VkSwapchainCreateInfoKHR SwapchainCreateInfo;
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.pNext = nullptr;
	SwapchainCreateInfo.flags = 0;
	SwapchainCreateInfo.surface = Context.Surface;
	SwapchainCreateInfo.minImageCount = Description.BackBufferCount;
	SwapchainCreateInfo.imageFormat = Description.BackBufferFormat;
	SwapchainCreateInfo.imageColorSpace = Description.ColorSpace;
	SwapchainCreateInfo.imageExtent = SurfaceProperties.Capabilities.currentExtent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapchainCreateInfo.preTransform = SurfaceProperties.Capabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.queueFamilyIndexCount = 0;
	SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
	SwapchainCreateInfo.presentMode = Description.PresentMode;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	Extent = SwapchainCreateInfo.imageExtent;

	const VkResult Result = vkCreateSwapchainKHR(Context.GraphicsDevice->Device, &SwapchainCreateInfo, Context.Allocator, &Swapchain);
	VERIFY(Result == VK_SUCCESS, "Failed to create swapchain");
}

void FVulkanGraphicsSwapchain::DestroySwapchain() const
{
	vkDestroySwapchainKHR(Context.GraphicsDevice->Device, Swapchain, Context.Allocator);
}

void FVulkanGraphicsSwapchain::AcquireBackBuffers()
{
	uint32 BackBufferCount = 0;
	VkResult Result = vkGetSwapchainImagesKHR(Context.GraphicsDevice->Device, Swapchain, &BackBufferCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to get swapchain images");
	BackBuffers.resize(BackBufferCount);
	Result = vkGetSwapchainImagesKHR(Context.GraphicsDevice->Device, Swapchain, &BackBufferCount, BackBuffers.data());
	VERIFY(Result == VK_SUCCESS, "Failed to get swapchain images");

	BackBufferViews.resize(BackBufferCount);
	for (uint32 i = 0; i < BackBufferCount; ++i)
	{
		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.image = BackBuffers[i];
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
		Result = vkCreateImageView(Context.GraphicsDevice->Device, &ImageViewCreateInfo, Context.Allocator, &BackBufferViews[i]);
		VERIFY(Result == VK_SUCCESS, "Failed to create image view");
	}
}

void FVulkanGraphicsSwapchain::ReleaseBackBuffers() const
{
	for (const VkImageView BackBufferView : BackBufferViews)
		vkDestroyImageView(Context.GraphicsDevice->Device, BackBufferView, Context.Allocator);
}
