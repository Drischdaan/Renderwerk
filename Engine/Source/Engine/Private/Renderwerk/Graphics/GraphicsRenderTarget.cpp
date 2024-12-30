#include "pch.h"

#include "Renderwerk/Graphics/GraphicsRenderTarget.h"

FGraphicsRenderTarget::FGraphicsRenderTarget(const TSharedPtr<FGraphicsContext>& InContext)
	: Context(InContext)
{
}

FGraphicsRenderTarget::~FGraphicsRenderTarget() = default;

void FGraphicsRenderTarget::Initialize(const FGraphicsRenderTargetDesc& InDescription)
{
	Description = InDescription;

	VkImageCreateInfo ImageCreateInfo = Vulkan::CreateStructure<VkImageCreateInfo>();
	ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageCreateInfo.extent.width = Description.Width;
	ImageCreateInfo.extent.height = Description.Height;
	ImageCreateInfo.extent.depth = 1;
	ImageCreateInfo.mipLevels = 1;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.format = Description.Format;
	ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

	VmaAllocationCreateInfo AllocationCreateInfo = Vulkan::CreateStructure<VmaAllocationCreateInfo>();
	AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	AllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	VkResult Result = vmaCreateImage(Context->ResourceAllocator, &ImageCreateInfo, &AllocationCreateInfo, &Image, &Allocation, &AllocationInfo);
	ASSERT(Result == VK_SUCCESS, "Failed to create image");

	VkImageViewCreateInfo ImageViewCreateInfo = Vulkan::CreateStructure<VkImageViewCreateInfo>();
	ImageViewCreateInfo.image = Image;
	ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewCreateInfo.format = Description.Format;
	ImageViewCreateInfo.subresourceRange.aspectMask = Description.AspectMask;
	ImageViewCreateInfo.subresourceRange.levelCount = 1;
	ImageViewCreateInfo.subresourceRange.layerCount = 1;
	ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	Result = vkCreateImageView(Context->Device, &ImageViewCreateInfo, Context->Allocator, &ImageView);
	ASSERT(Result == VK_SUCCESS, "Failed to create image view");
}

void FGraphicsRenderTarget::Destroy() const
{
	vkDestroyImageView(Context->Device, ImageView, Context->Allocator);
	vmaDestroyImage(Context->ResourceAllocator, Image, Allocation);
}
