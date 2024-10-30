#include "pch.h"

#include "Renderwerk/RHI/RHICommon.h"

FString ToString(const ERHIType InType)
{
	switch (InType)
	{
	ENUM_CASE(ERHIType, None);
	ENUM_CASE(ERHIType, D3D12);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EAdapterFeatureType Type)
{
	switch (Type)
	{
	ENUM_CASE(EAdapterFeatureType, None);
	ENUM_CASE(EAdapterFeatureType, Raytracing);
	ENUM_CASE(EAdapterFeatureType, VariableRateShading);
	ENUM_CASE(EAdapterFeatureType, MeshShading);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EAdapterType Type)
{
	switch (Type)
	{
	ENUM_CASE(EAdapterType, None);
	ENUM_CASE(EAdapterType, Software);
	ENUM_CASE(EAdapterType, Discrete);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EAdapterVendor Vendor)
{
	switch (Vendor)
	{
	ENUM_CASE(EAdapterVendor, None);
	ENUM_CASE(EAdapterVendor, NVIDIA);
	ENUM_CASE(EAdapterVendor, AMD);
	ENUM_CASE(EAdapterVendor, Intel);
	ENUM_CASE(EAdapterVendor, Microsoft);
	default:
		return TEXT("Unknown");
	}
}
