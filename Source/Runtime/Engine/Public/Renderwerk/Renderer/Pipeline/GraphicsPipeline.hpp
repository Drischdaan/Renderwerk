#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/Map.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/ShaderCompiler.hpp"
#include "Renderwerk/Renderer/Pipeline/Pipeline.hpp"
#include "Renderwerk/Renderer/Pipeline/PipelineCommon.hpp"

struct ENGINE_API FGraphicsPipelineDesc
{
	ECullMode CullMode = ECullMode::Back;
	EFillMode FillMode = EFillMode::Solid;
	EDepthOperation DepthOperation = EDepthOperation::None;
	TVector<DXGI_FORMAT> Formats;
	DXGI_FORMAT DepthFormat = DXGI_FORMAT_UNKNOWN;
	bool8 bHasDepth = false;
	bool8 bEnableDepthWrite = false;
	bool8 bIsCCW = true;
	bool8 bUseShaderReflection = false;
	TObjectHandle<FRootSignature> RootSignature = nullptr;
	TMap<EShaderType, FShaderModule> Shaders;
};

class ENGINE_API FGraphicsPipeline : public IPipeline
{
public:
	FGraphicsPipeline(FDevice* InDevice, const FGraphicsPipelineDesc& InGraphicsPipelineDesc);
	~FGraphicsPipeline() override;

	NON_COPY_MOVEABLE(FGraphicsPipeline)

public:

private:
	FGraphicsPipelineDesc GraphicsPipelineDesc;
};
