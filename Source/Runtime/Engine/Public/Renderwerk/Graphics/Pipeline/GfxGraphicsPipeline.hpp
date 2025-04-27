#pragma once

#include "Renderwerk/Core/Containers/Map.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxShaderCompiler.hpp"
#include "Renderwerk/Graphics/Pipeline/GfxPipeline.hpp"

enum class ENGINE_API EGfxCullMode : uint8
{
	None = D3D12_CULL_MODE_NONE,
	Back = D3D12_CULL_MODE_BACK,
	Front = D3D12_CULL_MODE_FRONT,
};

enum class ENGINE_API EGfxFillMode : uint8
{
	Solid = D3D12_FILL_MODE_SOLID,
	Wireframe = D3D12_FILL_MODE_WIREFRAME,
};

enum class ENGINE_API EGfxDepthOperation : uint8
{
	None = D3D12_COMPARISON_FUNC_NONE,
	Greater = D3D12_COMPARISON_FUNC_GREATER,
	Less = D3D12_COMPARISON_FUNC_LESS,
	Equal = D3D12_COMPARISON_FUNC_EQUAL,
	LessEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,
};

struct ENGINE_API FGfxGraphicsPipelineDesc
{
	EGfxCullMode CullMode = EGfxCullMode::Back;
	EGfxFillMode FillMode = EGfxFillMode::Solid;
	EGfxDepthOperation DepthOperation = EGfxDepthOperation::None;
	TVector<DXGI_FORMAT> Formats;
	DXGI_FORMAT DepthFormat = DXGI_FORMAT_UNKNOWN;
	bool8 bHasDepth = false;
	bool8 bEnableDepthWrite = false;
	bool8 bIsCCW = true;
	bool8 bUseShaderReflection = false;
	TRef<FGfxRootSignature> RootSignature = nullptr;
	TMap<EGfxShaderType, FGfxShaderModule> Shaders;
};

class ENGINE_API FGfxGraphicsPipeline : public IGfxPipeline
{
public:
	explicit FGfxGraphicsPipeline(FGfxDevice* InGfxDevice, const FGfxGraphicsPipelineDesc& InGraphicsPipelineDesc);
	FGfxGraphicsPipeline(FGfxDevice* InGfxDevice, const FGfxGraphicsPipelineDesc& InGraphicsPipelineDesc, const FStringView& InDebugName);
	~FGfxGraphicsPipeline() override;

	NON_COPY_MOVEABLE(FGfxGraphicsPipeline)

public:

private:
	FGfxGraphicsPipelineDesc GraphicsPipelineDesc;
};
