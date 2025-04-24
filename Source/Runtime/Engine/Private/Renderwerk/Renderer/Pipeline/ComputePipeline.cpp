#include "pch.hpp"

#include "Renderwerk/Renderer/Pipeline/ComputePipeline.hpp"

#include "Renderwerk/Renderer/Device.hpp"

FComputePipeline::FComputePipeline(FDevice* InDevice, const FShaderModule& Shader, const TObjectHandle<FRootSignature>& InRootSignature)
	: IPipeline(InDevice)
{
	RootSignature = InRootSignature;

	D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineStateDesc = {};
	PipelineStateDesc.CS.pShaderBytecode = Shader.ByteCode.data();
	PipelineStateDesc.CS.BytecodeLength = Shader.ByteCode.size();
	PipelineStateDesc.pRootSignature = RootSignature->GetHandle();

	const HRESULT Result = Device->GetHandle()->CreateComputePipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState));
	RW_VERIFY_ID(Result);
}

FComputePipeline::~FComputePipeline() = default;
