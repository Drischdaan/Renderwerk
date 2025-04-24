#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/RootSignature.hpp"

class ENGINE_API IPipeline : public TGraphicsObject<IPipeline>
{
public:
	IPipeline(FDevice* InDevice)
		: Device(InDevice)
	{
	}

	virtual ~IPipeline() override
	{
		RootSignature.Reset();
		PipelineState.Reset();
	}

	NON_COPY_MOVEABLE(IPipeline)

public:
	[[nodiscard]] TObjectHandle<ID3D12PipelineState> GetHandle() const { return PipelineState; }

	[[nodiscard]] TObjectHandle<FRootSignature> GetRootSignature() const { return RootSignature; }

protected:
	FDevice* Device;

	TObjectHandle<ID3D12PipelineState> PipelineState;
	TObjectHandle<FRootSignature> RootSignature;
};
