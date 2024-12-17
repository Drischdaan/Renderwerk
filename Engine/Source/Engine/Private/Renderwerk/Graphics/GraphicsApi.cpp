#include "pch.h"

#include "Renderwerk/Graphics/GraphicsApi.h"

#include "Renderwerk/Platform/Window.h"

#define VOLK_IMPLEMENTATION
#include <volk.h>

FGraphicsApi::FGraphicsApi() = default;

FGraphicsApi::~FGraphicsApi() = default;

void FGraphicsApi::Initialize()
{
	GraphicsContext = MakeShared<FGraphicsContext>();
	GraphicsContext->Initialize();
}

void FGraphicsApi::Destroy() const
{
	GraphicsContext->Destroy();
}

VkSurfaceKHR FGraphicsApi::CreateSurface(const TSharedPtr<FWindow>& Window) const
{
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	SurfaceCreateInfo.hwnd = Window->GetHandle();

	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	const FVulkanResult Result = vkCreateWin32SurfaceKHR(GraphicsContext->GetInstance(), &SurfaceCreateInfo, GraphicsContext->GetAllocator(), &Surface);
	ASSERT(Result == VK_SUCCESS, "Failed to create surface");
	return Surface;
}

TSharedPtr<FGraphicsAdapter> FGraphicsApi::ChooseCompatibleAdapter(const VkSurfaceKHR& Surface) const
{
	uint32 AdapterCount = 0;
	FVulkanResult Result = vkEnumeratePhysicalDevices(GraphicsContext->GetInstance(), &AdapterCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate physical devices");
	TVector<VkPhysicalDevice> PhysicalDevices(AdapterCount);
	Result = vkEnumeratePhysicalDevices(GraphicsContext->GetInstance(), &AdapterCount, PhysicalDevices.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate physical devices");
	for (VkPhysicalDevice PhysicalDevice : PhysicalDevices)
	{
		TSharedPtr<FGraphicsAdapter> Adapter = MakeShared<FGraphicsAdapter>();
		Adapter->Initialize(GraphicsContext, PhysicalDevice, Surface);
		if (Adapter->GetType() != EGraphicsAdapterType::Discrete)
			continue;
		if (!Adapter->SupportsExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
			continue;
		return Adapter;
	}
	ASSERT(false, "Failed to find compatible adapter");
	return {};
}

TSharedPtr<FGraphicsDevice> FGraphicsApi::CreateDevice(const TSharedPtr<FGraphicsAdapter>& Adapter) const
{
	TSharedPtr<FGraphicsDevice> GraphicsDevice = MakeShared<FGraphicsDevice>();
	GraphicsDevice->Initialize(GraphicsContext, Adapter);
	return GraphicsDevice;
}

TSharedPtr<FGraphicsSwapchain> FGraphicsApi::CreateSwapchain(const TSharedPtr<FGraphicsDevice>& Device) const
{
	return MakeShared<FGraphicsSwapchain>(GraphicsContext, Device);
}

void FGraphicsApi::DestroySurface(const VkSurfaceKHR& Surface) const
{
	vkDestroySurfaceKHR(GraphicsContext->GetInstance(), Surface, GraphicsContext->GetAllocator());
}
