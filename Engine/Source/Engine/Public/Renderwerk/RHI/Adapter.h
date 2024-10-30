#pragma once

#include "Renderwerk/RHI/RHICommon.h"

struct RENDERWERK_API FAdapterFeature
{
	EAdapterFeatureType Type;
	uint32 Version;
};

struct RENDERWERK_API FAdapterCapabilities
{
	uint32 AvailableVideoMemory = 0;
	TVector<FAdapterFeature> SupportedFeatures;
};

class RENDERWERK_API IAdapter
{
public:
	IAdapter(const uint32& InIndex)
		: Index(InIndex)
	{
	}

	virtual ~IAdapter() = default;

	DELETE_COPY_AND_MOVE(IAdapter);

public:
	NODISCARD INLINE bool8 SupportsFeature(EAdapterFeatureType Type)
	{
		return std::ranges::find_if(Capabilities.SupportedFeatures, [Type](const FAdapterFeature& Feature) { return Feature.Type == Type; }) != Capabilities.
			SupportedFeatures.end();
	}

	NODISCARD INLINE uint32 GetFeatureVersion(EAdapterFeatureType Type)
	{
		const auto Feature = std::ranges::find_if(Capabilities.SupportedFeatures, [Type](const FAdapterFeature& Feature) { return Feature.Type == Type; });
		return Feature != Capabilities.SupportedFeatures.end() ? Feature->Version : 0;
	}

public:
	NODISCARD virtual FString GetName() const = 0;
	NODISCARD virtual EAdapterType GetType() const = 0;
	NODISCARD virtual EAdapterVendor GetVendor() const = 0;

public:
	NODISCARD uint32 GetIndex() const { return Index; }
	NODISCARD FAdapterCapabilities GetCapabilities() const { return Capabilities; }

protected:
	uint32 Index = 0;
	FAdapterCapabilities Capabilities = {};
};
