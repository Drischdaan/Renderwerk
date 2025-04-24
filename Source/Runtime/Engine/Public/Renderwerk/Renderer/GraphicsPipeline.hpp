#pragma once

#include "RootSignature.hpp"
#include "ShaderCompiler.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/Map.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FDevice;

enum class ENGINE_API ECullMode : uint8
{
	None = D3D12_CULL_MODE_NONE,
	Back = D3D12_CULL_MODE_BACK,
	Front = D3D12_CULL_MODE_FRONT,
};

enum class ENGINE_API EFillMode : uint8
{
	Solid = D3D12_FILL_MODE_SOLID,
	Wireframe = D3D12_FILL_MODE_WIREFRAME,
};

enum class ENGINE_API EDepthOperation : uint8
{
	None = D3D12_COMPARISON_FUNC_NONE,
	Greater = D3D12_COMPARISON_FUNC_GREATER,
	Less = D3D12_COMPARISON_FUNC_LESS,
	Equal = D3D12_COMPARISON_FUNC_EQUAL,
	LessEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,
};

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

class ENGINE_API FGraphicsPipeline : public TGraphicsObject<FGraphicsPipeline>
{
public:
	FGraphicsPipeline(FDevice* InDevice, const FGraphicsPipelineDesc& InGraphicsPipelineDesc);
	~FGraphicsPipeline() override;

	NON_COPY_MOVEABLE(FGraphicsPipeline)

public:
	[[nodiscard]] TObjectHandle<ID3D12PipelineState> GetHandle() const { return PipelineState; }

	[[nodiscard]] TObjectHandle<FRootSignature> GetRootSignature() const { return RootSignature; }

private:
	FDevice* Device;
	FGraphicsPipelineDesc GraphicsPipelineDesc;

	TObjectHandle<ID3D12PipelineState> PipelineState;
	TObjectHandle<FRootSignature> RootSignature;
};
