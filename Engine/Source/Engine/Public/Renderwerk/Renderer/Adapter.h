#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include <vulkan/vulkan.h>

enum class ENGINE_API EAdapterType : uint8
{
	Unknown = 0,
	Discrete,
	Integrated,
};

ENGINE_API FString ToString(EAdapterType Type);

// https://pcisig.com/membership/member-companies
enum class ENGINE_API EAdapterVendor : uint16
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

ENGINE_API FString ToString(EAdapterVendor Vendor);

enum class ENGINE_API EQueueType : uint8
{
	Graphics = 0,
	Compute,
	Transfer,
	Present,
};

ENGINE_API FString ToString(EQueueType Type);

struct ENGINE_API FQueueInfo
{
	EQueueType Type;
	uint32 FamilyIndex;
	VkQueueFamilyProperties Properties;
};

class ENGINE_API FAdapter
{
public:
	FAdapter(uint32 Index, VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
	~FAdapter();

	DELETE_COPY_AND_MOVE(FAdapter);

public:
	[[nodiscard]] bool8 SupportsExtension(const FString& Extension) const;
	[[nodiscard]] bool8 SupportsLayer(const FString& Layer) const;

	[[nodiscard]] FQueueInfo GetQueueInfo(EQueueType Type) const;

public:
	[[nodiscard]] uint32 GetIndex() const { return Index; }

	[[nodiscard]] FString GetName() const { return Name; }
	[[nodiscard]] EAdapterType GetType() const { return Type; }
	[[nodiscard]] EAdapterVendor GetVendor() const { return Vendor; }
	[[nodiscard]] uint32 GetApiVersion() const { return Properties.apiVersion; }
	[[nodiscard]] uint32 GetDriverVersion() const { return Properties.driverVersion; }

private:
	void QueryQueueInfos(VkSurfaceKHR Surface);

private:
	static bool8 HasMatch(const TVector<FString>& Available, const FString& Required);

private:
	uint32 Index;
	VkPhysicalDevice PhysicalDevice;

	VkPhysicalDeviceProperties Properties;

	FString Name;
	EAdapterType Type;
	EAdapterVendor Vendor;

	TVector<FString> Extensions;
	TVector<FString> Layers;

	TMap<EQueueType, FQueueInfo> QueueInfos;
};
