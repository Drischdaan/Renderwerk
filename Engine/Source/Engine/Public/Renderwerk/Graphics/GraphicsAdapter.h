#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

enum class ENGINE_API EGraphicsAdapterType : uint8
{
	Unknown = 0,
	Integrated,
	Discrete,
	Virtual,
	Software,
};

// https://pcisig.com/membership/member-companies
enum class ENGINE_API EGraphicsAdapterVendor : uint16
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

[[nodiscard]] ENGINE_API FString GetVendorString(EGraphicsAdapterVendor Vendor);

struct ENGINE_API FGraphicsAdapterProperties
{
	FString Name;
	EGraphicsAdapterType Type = EGraphicsAdapterType::Unknown;
	EGraphicsAdapterVendor Vendor = EGraphicsAdapterVendor::Unknown;
	uint32 ApiVersion = 0;
	uint32 DriverVersion = 0;
};

class ENGINE_API FGraphicsAdapter
{
public:
	FGraphicsAdapter(const TSharedPtr<FGraphicsContext>& InContext, VkPhysicalDevice InPhysicalDevice);
	~FGraphicsAdapter();

	DELETE_COPY_AND_MOVE(FGraphicsAdapter);

public:
	void Initialize(VkSurfaceKHR Surface);

	[[nodiscard]] bool8 IsLayerSupported(FStringView LayerName) const;
	[[nodiscard]] bool8 IsExtensionSupported(FStringView ExtensionName) const;

	[[nodiscard]] FString GetDriverVersionString() const;

	[[nodiscard]] FGraphicsQueueMetadata GetQueueMetadata(EGraphicsQueueType QueueType) const;
	[[nodiscard]] FGraphicsSurfaceCapabilities GetSurfaceCapabilities(VkSurfaceKHR Surface) const;

public:
	[[nodiscard]] FGraphicsAdapterProperties GetProperties() const { return Properties; }

	[[nodiscard]] TMap<EGraphicsQueueType, FGraphicsQueueMetadata> GetQueueMetadataMap() const { return QueueMetadata; }

private:
	TSharedPtr<FGraphicsContext> Context;

	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

	FGraphicsAdapterProperties Properties = {};

	TVector<FString> Layers;
	TVector<FString> Extensions;

	TMap<EGraphicsQueueType, FGraphicsQueueMetadata> QueueMetadata;
};
