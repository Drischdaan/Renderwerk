#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FGraphicsBackendDesc
{
	TVector<const char*> AdditionalLayers = {};
	TVector<const char*> AdditionalExtensions = {};
};

/**
 * The graphics backend is the primary entry point for the graphics system. 
 * It manages the core system components, including:
 *   - Graphics system instance (e.g., Vulkan VkInstance, DirectX12 IDXGIFactory)
 *   - Related debugging objects (e.g., Vulkan VkDebugUtilsMessengerEXT, DirectX12 IDXGIDebug/ID3D12Debug)
 *   - Graphics adapter acquisition (e.g., Vulkan VkPhysicalDevice, DirectX12 IDXGIAdapter)
 *   - Creation of the graphics device (e.g., Vulkan VkDevice, DirectX12 ID3D12Device)
 *   - Other relevant second level objects.
 */
class ENGINE_API FGraphicsBackend
{
private:
	struct ENGINE_API FInstanceProperties
	{
		uint32 ApiVersion = VK_API_VERSION_1_0;
		TVector<FString> Layers;
		TVector<FString> Extensions;
	};

public:
	FGraphicsBackend();
	~FGraphicsBackend();

	DELETE_COPY_AND_MOVE(FGraphicsBackend);

public:
	void Initialize(const FGraphicsBackendDesc& InDescription);
	void Destroy();

	[[nodiscard]] VkSurfaceKHR CreateSurface(const TSharedPtr<FWindow>& Window) const;
	void DestroySurface(VkSurfaceKHR Surface) const;

	[[nodiscard]] TVector<TSharedPtr<FGraphicsAdapter>> GetAdapters();
	[[nodiscard]] bool8 IsAdapterSuitable(const TSharedPtr<FGraphicsAdapter>& Adapter, const TSpan<const char*>& RequiredExtensions) const;

	[[nodiscard]] TSharedPtr<FGraphicsDevice> CreateDevice(const TSharedPtr<FGraphicsAdapter>& Adapter) const;

private:
	static VkAllocationCallbacks* CreateAllocator();
	static FInstanceProperties QueryInstanceProperties();
	static void ValidateRequiredInstanceObjects(FString ObjectName, TSpan<FString> AvailableObjects, TSpan<const char*> RequiredObjects);
	static VkInstance CreateInstance(const TSharedPtr<FGraphicsContext>& Context, FInstanceProperties& InProperties, TSpan<const char*> RequiredLayers = {},
	                                 TSpan<const char*> RequiredExtensions = {});

private:
	FGraphicsBackendDesc Description = {};

	FInstanceProperties InstanceProperties = {};
	TSharedPtr<FGraphicsContext> Context;
};
