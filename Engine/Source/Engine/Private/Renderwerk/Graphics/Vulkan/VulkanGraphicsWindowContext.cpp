#include "pch.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsWindowContext.h"

#include "Renderwerk/Graphics/Vulkan/VulkanGraphicsBackend.h"
#include "Renderwerk/Platform/Window.h"

FVulkanGraphicsWindowContext::FVulkanGraphicsWindowContext(IGraphicsBackend* InBackend)
	: IGraphicsWindowContext(InBackend)
{
}

FVulkanGraphicsWindowContext::~FVulkanGraphicsWindowContext() = default;

void FVulkanGraphicsWindowContext::Initialize(const TSharedPtr<FWindow>& InWindow)
{
	Window = InWindow;

	const FVulkanGraphicsBackend& VulkanBackend = Backend->As<FVulkanGraphicsBackend>();

	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	SurfaceCreateInfo.hwnd = Window->GetHandle();

	const VkResult Result = vkCreateWin32SurfaceKHR(VulkanBackend.GetVulkanInstance(), &SurfaceCreateInfo, VulkanBackend.GetVulkanAllocator(), &Surface);
	ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan surface");

	RW_LOG(LogGraphics, Debug, "Created vulkan window context for window '{}' ('{}')", Window->GetId(), Window->GetState().Title);
}

void FVulkanGraphicsWindowContext::Destroy()
{
	const FVulkanGraphicsBackend& VulkanBackend = Backend->As<FVulkanGraphicsBackend>();
	vkDestroySurfaceKHR(VulkanBackend.GetVulkanInstance(), Surface, VulkanBackend.GetVulkanAllocator());
	Window.reset();
}
