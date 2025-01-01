#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

enum class ENGINE_API EGraphicsAdapterType : uint8
{
	Unknown = 0,
	Discrete = 1,
	Integrated = 2,
	Software = 3,
	Virtual = 4,
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

[[nodiscard]] constexpr ENGINE_API FString GetGraphicsAdapterVendorName(const EGraphicsAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EGraphicsAdapterVendor::NVIDIA: return TEXT("NVIDIA");
	case EGraphicsAdapterVendor::AMD: return TEXT("AMD");
	case EGraphicsAdapterVendor::Intel: return TEXT("Intel");
	case EGraphicsAdapterVendor::Unknown:
	default:
		return TEXT("Unknown");
	}
}

[[nodiscard]] constexpr ENGINE_API EGraphicsAdapterVendor MapAdapterVendor(const uint32& InVendorID)
{
	switch (InVendorID)
	{
	case static_cast<uint32>(EGraphicsAdapterVendor::NVIDIA):
		return EGraphicsAdapterVendor::NVIDIA;
	case static_cast<uint32>(EGraphicsAdapterVendor::AMD):
		return EGraphicsAdapterVendor::AMD;
	case static_cast<uint32>(EGraphicsAdapterVendor::Intel):
		return EGraphicsAdapterVendor::Intel;
	case static_cast<uint32>(EGraphicsAdapterVendor::Microsoft):
		return EGraphicsAdapterVendor::Microsoft;
	default:
		return EGraphicsAdapterVendor::Unknown;
	}
}

struct ENGINE_API FGraphicsAdapterProperties
{
	FString Name;
	uint32 DeviceId;
	EGraphicsAdapterType Type;
	EGraphicsAdapterVendor Vendor;
	uint32 DriverVersion;
};

class ENGINE_API IGraphicsAdapter : public TGraphicsObject<IGraphicsAdapter>, public IGraphicsBackendChild
{
public:
	IGraphicsAdapter(IGraphicsBackend* GraphicsBackend)
		: IGraphicsBackendChild(GraphicsBackend)
	{
	}

	virtual ~IGraphicsAdapter() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsAdapter);

public:
	virtual void Initialize(const TSharedPtr<IGraphicsWindowContext>& InGraphicsWindowContext) = 0;
	virtual void Destroy() = 0;

public:
	[[nodiscard]] FString GetDriverVersionString() const;

public:
	[[nodiscard]] FGraphicsAdapterProperties GetProperties() const { return Properties; }

protected:
	TSharedPtr<IGraphicsWindowContext> GraphicsWindowContext;

	FGraphicsAdapterProperties Properties = {};
};

inline FString IGraphicsAdapter::GetDriverVersionString() const
{
	FString DriverVersionString;
	switch (Properties.Vendor)
	{
	case EGraphicsAdapterVendor::NVIDIA:
		DriverVersionString = std::format("{}.{}", (Properties.DriverVersion >> 22) & 0x3ff, (Properties.DriverVersion >> 14) & 0x0ff);
		break;
	case EGraphicsAdapterVendor::Intel:
		DriverVersionString = std::format("{}.{}", Properties.DriverVersion >> 14, Properties.DriverVersion & 0x3fff);
		break;
	case EGraphicsAdapterVendor::Microsoft:
	case EGraphicsAdapterVendor::AMD:
	case EGraphicsAdapterVendor::Unknown:
	default:
		DriverVersionString = std::format("{}.{}.{}", VERSION_MAJOR(Properties.DriverVersion), VERSION_MAJOR(Properties.DriverVersion),
		                                  VERSION_MAJOR(Properties.DriverVersion));
		break;
	}
	return DriverVersionString;
}
