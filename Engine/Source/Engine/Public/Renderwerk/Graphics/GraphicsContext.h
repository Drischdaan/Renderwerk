#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API FGraphicsContext
{
public:
	FGraphicsContext();
	~FGraphicsContext();

	DEFINE_DEFAULT_COPY_AND_MOVE(FGraphicsContext);

public:
	void Initialize();
	void Destroy() const;

public:
	[[nodiscard]] uint32 GetApiVersion() const { return ApiVersion; }
	[[nodiscard]] const VkAllocationCallbacks* GetAllocator() const { return &Allocator; }
	[[nodiscard]] VkInstance GetInstance() const { return Instance; }

private:
	void AcquireApiVersion();
	void CreateAllocator();
	void CreateInstance();

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	void CreateDebugMessenger();
#endif

private:
	static void CheckInstanceExtensions(const TVector<const char*>& RequiredExtensions);
	static void CheckInstanceLayers(const TVector<const char*>& RequiredExtensions);

private:
	uint32 ApiVersion = 0;

	VkAllocationCallbacks Allocator = {};
	VkInstance Instance = VK_NULL_HANDLE;

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
#endif
};
