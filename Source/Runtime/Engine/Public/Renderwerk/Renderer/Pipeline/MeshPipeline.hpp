#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/Pipeline/GraphicsPipeline.hpp"
#include "Renderwerk/Renderer/Pipeline/Pipeline.hpp"

class ENGINE_API FMeshPipeline : public IPipeline
{
public:
	FMeshPipeline(FDevice* InDevice, const FGraphicsPipelineDesc& InGraphicsPipelineDesc);
	~FMeshPipeline() override;

	NON_COPY_MOVEABLE(FMeshPipeline)

private:
	FGraphicsPipelineDesc GraphicsPipelineDesc;
};
