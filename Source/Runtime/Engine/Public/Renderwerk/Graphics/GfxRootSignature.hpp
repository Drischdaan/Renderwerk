#pragma once

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

enum class ENGINE_API EGfxRootType : uint8
{
	PushConstant = 0xFF,
	ConstantBuffer = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
	ShaderResource = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
	Storage = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
	Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
};

class ENGINE_API FGfxRootSignature : public IGfxDeviceChild
{
public:
	explicit FGfxRootSignature(FGfxDevice* InGfxDevice);
	explicit FGfxRootSignature(FGfxDevice* InGfxDevice, const TVector<EGfxRootType>& InRootTypes, size64 InPushConstantSize);
	FGfxRootSignature(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	FGfxRootSignature(FGfxDevice* InGfxDevice, const TVector<EGfxRootType>& InRootTypes, size64 InPushConstantSize, const FStringView& InDebugName);
	~FGfxRootSignature() override;

	NON_COPY_MOVEABLE(FGfxRootSignature)

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

private:
	TComPtr<ID3D12RootSignature> RootSignature;
};
