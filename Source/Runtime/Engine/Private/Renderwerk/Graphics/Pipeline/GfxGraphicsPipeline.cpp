﻿#include "pch.hpp"

#include "Renderwerk/Graphics/Pipeline/GfxGraphicsPipeline.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxRootSignature.hpp"

constexpr uint32 GetDXGIFormatSize(const DXGI_FORMAT Format)
{
	switch (Format)
	{
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R32_FLOAT:
		return 4;
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G32_FLOAT:
		return 8;
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return 12;
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return 16;
	case DXGI_FORMAT_UNKNOWN:
	default:
		return 0;
	}
}

FGfxGraphicsPipeline::FGfxGraphicsPipeline(FGfxDevice* InGfxDevice, const FGfxGraphicsPipelineDesc& InGraphicsPipelineDesc)
	: FGfxGraphicsPipeline(InGfxDevice, InGraphicsPipelineDesc, TEXT("UnnamedGraphicsPipeline"))
{
}

FGfxGraphicsPipeline::FGfxGraphicsPipeline(FGfxDevice* InGfxDevice, const FGfxGraphicsPipelineDesc& InGraphicsPipelineDesc, const FStringView& InDebugName)
	: IGfxPipeline(InGfxDevice, InDebugName), GraphicsPipelineDesc(InGraphicsPipelineDesc)
{
	RW_VERIFY_MSG(GraphicsPipelineDesc.Shaders.contains(EGfxShaderType::Vertex), "No vertex shader given");
	RW_VERIFY_MSG(GraphicsPipelineDesc.Shaders.contains(EGfxShaderType::Pixel), "No vertex shader given");

	RootSignature = GraphicsPipelineDesc.RootSignature;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
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
	if (RootSignature)
	{
		ID3D12RootSignature* NativeRootSignature = RootSignature->GetNativeObject<ID3D12RootSignature>(NativeObjectIds::D3D12_RootSignature);
		PipelineStateDesc.pRootSignature = NativeRootSignature;
	}

	FGfxShaderModule& VertexShaderModule = GraphicsPipelineDesc.Shaders.at(EGfxShaderType::Vertex);
	FGfxShaderModule& PixelShaderModule = GraphicsPipelineDesc.Shaders.at(EGfxShaderType::Pixel);

	PipelineStateDesc.VS.pShaderBytecode = VertexShaderModule.ByteCode.data();
	PipelineStateDesc.VS.BytecodeLength = VertexShaderModule.ByteCode.size();
	PipelineStateDesc.PS.pShaderBytecode = PixelShaderModule.ByteCode.data();
	PipelineStateDesc.PS.BytecodeLength = PixelShaderModule.ByteCode.size();

	TVector<D3D12_INPUT_ELEMENT_DESC> InputElementDescs;
	// ReSharper disable once CppTooWideScope
	TVector<FAnsiString> InputElementSemanticNames;
	if (GraphicsPipelineDesc.bUseShaderReflection)
	{
		TComPtr<ID3D12ShaderReflection> VertexShaderReflection = GfxDevice->GetShaderCompiler()->CreateShaderReflection(VertexShaderModule);
		D3D12_SHADER_DESC VertexShaderDesc;
		HRESULT Result = VertexShaderReflection->GetDesc(&VertexShaderDesc);
		RW_VERIFY_ID(Result);

		InputElementDescs.reserve(VertexShaderDesc.InputParameters);
		InputElementSemanticNames.reserve(VertexShaderDesc.InputParameters);

		for (uint32 ParameterIndex = 0; ParameterIndex < VertexShaderDesc.InputParameters; ++ParameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC ParameterDesc = {};
			Result = VertexShaderReflection->GetInputParameterDesc(ParameterIndex, &ParameterDesc);
			RW_VERIFY_ID(Result);

			InputElementSemanticNames.emplace_back(ParameterDesc.SemanticName);

			D3D12_INPUT_ELEMENT_DESC InputElement = {};
			InputElement.SemanticName = InputElementSemanticNames.back().c_str();
			InputElement.SemanticIndex = ParameterDesc.SemanticIndex;
			InputElement.InputSlot = 0;
			InputElement.AlignedByteOffset = ParameterIndex != 0 ? GetDXGIFormatSize(InputElementDescs.at(ParameterIndex - 1).Format) : 0;
			InputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			InputElement.InstanceDataStepRate = 0;

			if (ParameterDesc.Mask == 1)
			{
				if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32_UINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32_SINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElement.Format = DXGI_FORMAT_R32_FLOAT;
				}
			}
			else if (ParameterDesc.Mask <= 3)
			{
				if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32_UINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32_SINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32_FLOAT;
				}
			}
			else if (ParameterDesc.Mask <= 7)
			{
				if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32B32_UINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32B32_SINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				}
			}
			else if (ParameterDesc.Mask <= 15)
			{
				if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				}
				else if (ParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				}
			}
			InputElementDescs.push_back(InputElement);
		}
		PipelineStateDesc.InputLayout.pInputElementDescs = InputElementDescs.data();
		PipelineStateDesc.InputLayout.NumElements = static_cast<uint32_t>(InputElementDescs.size());
	}

	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	HRESULT Result = NativeDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState));
	RW_VERIFY_ID(Result);
}

FGfxGraphicsPipeline::~FGfxGraphicsPipeline() = default;
