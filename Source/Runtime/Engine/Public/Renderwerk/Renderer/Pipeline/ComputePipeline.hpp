#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/ShaderCompiler.hpp"
#include "Renderwerk/Renderer/Pipeline/Pipeline.hpp"

class ENGINE_API FComputePipeline : public IPipeline
{
public:
	FComputePipeline(FDevice* InDevice, const FShaderModule& Shader, const TObjectHandle<FRootSignature>& InRootSignature);
	~FComputePipeline() override;

	NON_COPY_MOVEABLE(FComputePipeline)

private:
};
