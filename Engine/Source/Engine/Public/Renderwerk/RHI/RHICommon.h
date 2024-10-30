#pragma once
// ReSharper disable CppUnusedIncludeDirective

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/RHI/RHIFwd.h"

enum class RENDERWERK_API ERHIType : uint8
{
	None = 0,
	D3D12,
	// Vulkan, // Not implemented yet, but will be added in the future
};

enum class RENDERWERK_API EAdapterFeatureType : uint8
{
	None = 0,
	Raytracing,
	VariableRateShading,
	MeshShading,
};

enum class RENDERWERK_API EAdapterType : uint8
{
	None = 0,
	Software,
	Discrete,
};

// https://pcisig.com/membership/member-companies
enum class RENDERWERK_API EAdapterVendor : uint16
{
	None = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

RENDERWERK_API FString ToString(ERHIType InType);
RENDERWERK_API FString ToString(EAdapterFeatureType Type);
RENDERWERK_API FString ToString(EAdapterType Type);
RENDERWERK_API FString ToString(EAdapterVendor Vendor);
