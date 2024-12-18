#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsContext.h"

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

enum class ENGINE_API EGraphicsAdapterType : uint8
{
	None = 0,
	Discrete,
	Integrated,
	Virtual,
	Software,
};

class ENGINE_API FGraphicsAdapter
{
public:
	FGraphicsAdapter();
	~FGraphicsAdapter();

	DELETE_COPY_AND_MOVE(FGraphicsAdapter);

public:
	void Initialize(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const VkPhysicalDevice& InPhysicalDevice, const VkSurfaceKHR& Surface);

	[[nodiscard]] bool8 SupportsExtension(const char* ExtensionName);
	[[nodiscard]] bool8 SupportsLayer(const char* LayerName);

	[[nodiscard]] FGraphicsQueueMetadata GetQueueMetadata(EGraphicsQueueType QueueType) const;
	[[nodiscard]] uint32 GetQueueCountForIndex(uint32 QueueFamilyIndex) const;

	[[nodiscard]] FGraphicsSurfaceProperties GetSurfaceProperties(const VkSurfaceKHR& Surface) const;

	[[nodiscard]] FString GetDriverVersionString() const;

public:
	[[nodiscard]] VkPhysicalDevice GetHandle() const { return PhysicalDevice; }

	[[nodiscard]] FString GetName() const { return Name; }
	[[nodiscard]] EGraphicsAdapterType GetType() const { return Type; }
	[[nodiscard]] EGraphicsAdapterVendor GetVendor() const { return Vendor; }
	[[nodiscard]] uint32 GetDriverVersion() const { return DriverVersion; }
	[[nodiscard]] uint32 GetApiVersion() const { return ApiVersion; }

private:
	void AcquireProperties();
	void AcquireExtensionsAndLayers();
	void AcquireQueueMetadata(const VkSurfaceKHR& Surface);

private:
	TSharedPtr<FGraphicsContext> GraphicsContext = nullptr;

	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

	FString Name = TEXT("Unknown");
	EGraphicsAdapterType Type = EGraphicsAdapterType::None;
	EGraphicsAdapterVendor Vendor = EGraphicsAdapterVendor::Unknown;
	uint32 DriverVersion = 0;
	uint32 ApiVersion = 0;

	TVector<VkExtensionProperties> SupportedExtensions;
	TVector<VkLayerProperties> SupportedLayers;

	TMap<EGraphicsQueueType, FGraphicsQueueMetadata> QueueMetadata;
};
