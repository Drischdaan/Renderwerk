#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FDevice;

enum class ENGINE_API ERootType : uint8
{
	PushConstant = 0xFF,
	ConstantBuffer = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
	ShaderResource = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
	Storage = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
	Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
};

class ENGINE_API FRootSignature : public TGraphicsObject<FRootSignature>
{
public:
	FRootSignature(FDevice* InDevice);
	FRootSignature(FDevice* InDevice, const TVector<ERootType>& RootTypes, size64 InPushConstantSize = 0);
	~FRootSignature() override;

	NON_COPY_MOVEABLE(FRootSignature)

public:
	[[nodiscard]] TObjectHandle<ID3D12RootSignature> GetHandle() const { return RootSignature; }

private:
	FDevice* Device;

	TObjectHandle<ID3D12RootSignature> RootSignature;
};
