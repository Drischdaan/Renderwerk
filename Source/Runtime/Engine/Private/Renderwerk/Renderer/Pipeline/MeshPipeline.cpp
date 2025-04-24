#include "pch.hpp"

#include "Renderwerk/Renderer/Pipeline/MeshPipeline.hpp"

#include "Renderwerk/Renderer/Device.hpp"

FMeshPipeline::FMeshPipeline(FDevice* InDevice, const FGraphicsPipelineDesc& InGraphicsPipelineDesc)
	: IPipeline(InDevice), GraphicsPipelineDesc(InGraphicsPipelineDesc)
{
	RW_VERIFY_MSG(GraphicsPipelineDesc.Shaders.contains(EShaderType::Mesh), "No mesh shader given");
	RW_VERIFY_MSG(GraphicsPipelineDesc.Shaders.contains(EShaderType::Pixel), "No vertex shader given");

	RootSignature = GraphicsPipelineDesc.RootSignature;

	FShaderModule& MeshShaderModule = GraphicsPipelineDesc.Shaders.at(EShaderType::Mesh);
	FShaderModule& PixelShaderModule = GraphicsPipelineDesc.Shaders.at(EShaderType::Pixel);

	D3DX12_MESH_SHADER_PIPELINE_STATE_DESC PipelineStateDesc = {};
	PipelineStateDesc.MS.pShaderBytecode = MeshShaderModule.ByteCode.data();
	PipelineStateDesc.MS.BytecodeLength = MeshShaderModule.ByteCode.size();
	PipelineStateDesc.PS.pShaderBytecode = PixelShaderModule.ByteCode.data();
	PipelineStateDesc.PS.BytecodeLength = PixelShaderModule.ByteCode.size();
	PipelineStateDesc.NumRenderTargets = static_cast<uint32>(GraphicsPipelineDesc.Formats.size());
	for (size64 Index = 0; Index < GraphicsPipelineDesc.Formats.size(); ++Index)
	{
		PipelineStateDesc.BlendState.RenderTarget[Index].SrcBlend = D3D12_BLEND_ONE;
		PipelineStateDesc.BlendState.RenderTarget[Index].DestBlend = D3D12_BLEND_ZERO;
		PipelineStateDesc.BlendState.RenderTarget[Index].BlendOp = D3D12_BLEND_OP_ADD;
		PipelineStateDesc.BlendState.RenderTarget[Index].SrcBlendAlpha = D3D12_BLEND_ONE;
		PipelineStateDesc.BlendState.RenderTarget[Index].DestBlendAlpha = D3D12_BLEND_ZERO;
		PipelineStateDesc.BlendState.RenderTarget[Index].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		PipelineStateDesc.BlendState.RenderTarget[Index].LogicOp = D3D12_LOGIC_OP_NOOP;
		PipelineStateDesc.BlendState.RenderTarget[Index].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		PipelineStateDesc.RTVFormats[Index] = GraphicsPipelineDesc.Formats.at(Index);
	}
	PipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	PipelineStateDesc.RasterizerState.FillMode = static_cast<D3D12_FILL_MODE>(GraphicsPipelineDesc.FillMode);
	PipelineStateDesc.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(GraphicsPipelineDesc.CullMode);
	PipelineStateDesc.RasterizerState.DepthClipEnable = false;
	PipelineStateDesc.RasterizerState.FrontCounterClockwise = GraphicsPipelineDesc.bIsCCW;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	if (GraphicsPipelineDesc.bHasDepth)
	{
		PipelineStateDesc.DepthStencilState.DepthEnable = true;
		PipelineStateDesc.DepthStencilState.DepthWriteMask = GraphicsPipelineDesc.bEnableDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		PipelineStateDesc.DepthStencilState.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(GraphicsPipelineDesc.DepthOperation);
		PipelineStateDesc.DSVFormat = GraphicsPipelineDesc.DepthFormat;
	}
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.SampleDesc.Quality = 0;
	if (GraphicsPipelineDesc.RootSignature)
	{
		PipelineStateDesc.pRootSignature = GraphicsPipelineDesc.RootSignature->GetHandle();
		RootSignature = GraphicsPipelineDesc.RootSignature;
	}

	CD3DX12_PIPELINE_MESH_STATE_STREAM MeshStateStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(PipelineStateDesc);

	D3D12_PIPELINE_STATE_STREAM_DESC PipelineStateStreamDesc;
	PipelineStateStreamDesc.pPipelineStateSubobjectStream = &MeshStateStream;
	PipelineStateStreamDesc.SizeInBytes = sizeof(MeshStateStream);

	HRESULT Result = Device->GetHandle()->CreatePipelineState(&PipelineStateStreamDesc, IID_PPV_ARGS(&PipelineState));
	RW_VERIFY_ID(Result);
}

FMeshPipeline::~FMeshPipeline() = default;
