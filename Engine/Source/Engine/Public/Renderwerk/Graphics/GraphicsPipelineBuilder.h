#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

// TODO: Only temporary, remove this
class ENGINE_API FGraphicsPipelineBuilder
{
public:
	FGraphicsPipelineBuilder();
	~FGraphicsPipelineBuilder() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FGraphicsPipelineBuilder);

public:
	void SetInputTopology(VkPrimitiveTopology Topology);
	void SetPolygonMode(VkPolygonMode PolygonMode);
	void SetCullMode(VkCullModeFlags CullMode, VkFrontFace FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);
	void SetColorAttachmentFormat(VkFormat Format);
	void SetDepthFormat(VkFormat Format);
	void SetPipelineLayout(VkPipelineLayout Layout);
	void AddShaderStage(VkShaderStageFlagBits Stage, VkShaderModule Module);

	VkPipeline BuildPipeline(const TSharedPtr<FGraphicsContext>& GraphicsContext, const TSharedPtr<FGraphicsDevice>& GraphicsDevice) const;

private:
	TVector<VkPipelineShaderStageCreateInfo> ShaderStages;

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyState;
	VkPipelineRasterizationStateCreateInfo RasterizationState;
	VkPipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
	VkPipelineMultisampleStateCreateInfo MultisampleState;
	VkPipelineDepthStencilStateCreateInfo DepthStencilState;
	VkPipelineRenderingCreateInfo RenderingCreateInfo;
	VkFormat ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
};
