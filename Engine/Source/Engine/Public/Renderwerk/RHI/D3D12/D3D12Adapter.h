#pragma once

#include "Renderwerk/RHI/Adapter.h"
#include "Renderwerk/RHI/D3D12/D3D12Common.h"

class FD3D12Adapter : public IAdapter
{
public:
	FD3D12Adapter(const uint32& InIndex, const TComPtr<IDXGIFactory7>& InFactory, const TComPtr<IDXGIAdapter4>& InAdapter);
	~FD3D12Adapter() override;

	DELETE_COPY_AND_MOVE(FD3D12Adapter);

public:
	NODISCARD TComPtr<IDXGIAdapter4> GetHandle() const { return Adapter; }
	NODISCARD TComPtr<IDXGIFactory7> GetFactory() const { return Factory; }

	NODISCARD FString GetName() const override { return Name; }
	NODISCARD EAdapterType GetType() const override { return Type; }
	NODISCARD EAdapterVendor GetVendor() const override { return Vendor; }

	NODISCARD D3D_FEATURE_LEVEL GetFeatureLevel() const { return FeatureLevel; }
	NODISCARD D3D_SHADER_MODEL GetShaderModel() const { return ShaderModel; }

private:
	TComPtr<IDXGIFactory7> Factory;
	TComPtr<IDXGIAdapter4> Adapter;

	FString Name;
	EAdapterType Type;
	EAdapterVendor Vendor;

	D3D_FEATURE_LEVEL FeatureLevel;
	D3D_SHADER_MODEL ShaderModel;
};
