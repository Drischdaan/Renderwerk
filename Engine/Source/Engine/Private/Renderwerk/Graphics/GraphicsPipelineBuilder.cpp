#include "pch.h"

#include "Renderwerk/Graphics/GraphicsPipelineBuilder.h"

FGraphicsPipelineBuilder::FGraphicsPipelineBuilder()
{
	InputAssemblyState = {};
	InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssemblyState.primitiveRestartEnable = VK_FALSE;

	RasterizationState = {};
	RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterizationState.lineWidth = 1.0f;
	RasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
	RasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	ColorBlendAttachmentState = {};
	ColorBlendAttachmentState.blendEnable = VK_FALSE;
	ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	MultisampleState = {};
	MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	MultisampleState.sampleShadingEnable = VK_FALSE;
	MultisampleState.minSampleShading = 1.0f;
	MultisampleState.alphaToCoverageEnable = VK_FALSE;
	MultisampleState.alphaToOneEnable = VK_FALSE;

	DepthStencilState = {};
	DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	DepthStencilState.depthTestEnable = VK_TRUE;
	DepthStencilState.depthWriteEnable = VK_TRUE;
	DepthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
	DepthStencilState.depthBoundsTestEnable = VK_FALSE;
	DepthStencilState.stencilTestEnable = VK_FALSE;
	DepthStencilState.minDepthBounds = 0.0f;
	DepthStencilState.maxDepthBounds = 1.0f;

	RenderingCreateInfo = {};
	RenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	RenderingCreateInfo.colorAttachmentCount = 1;
	RenderingCreateInfo.pColorAttachmentFormats = &ColorAttachmentFormat;
	RenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
}

void FGraphicsPipelineBuilder::SetInputTopology(const VkPrimitiveTopology Topology)
{
	InputAssemblyState.topology = Topology;
}

void FGraphicsPipelineBuilder::SetPolygonMode(const VkPolygonMode PolygonMode)
{
	RasterizationState.polygonMode = PolygonMode;
	RasterizationState.lineWidth = 1.0f;
}

void FGraphicsPipelineBuilder::SetCullMode(const VkCullModeFlags CullMode, const VkFrontFace FrontFace)
{
	RasterizationState.cullMode = CullMode;
	RasterizationState.frontFace = FrontFace;
}

void FGraphicsPipelineBuilder::SetColorAttachmentFormat(const VkFormat Format)
{
	ColorAttachmentFormat = Format;
	RenderingCreateInfo.colorAttachmentCount = 1;
	RenderingCreateInfo.pColorAttachmentFormats = &ColorAttachmentFormat;
}

void FGraphicsPipelineBuilder::SetDepthFormat(const VkFormat Format)
{
	RenderingCreateInfo.depthAttachmentFormat = Format;
}

void FGraphicsPipelineBuilder::SetPipelineLayout(const VkPipelineLayout Layout)
{
	PipelineLayout = Layout;
}

void FGraphicsPipelineBuilder::AddShaderStage(const VkShaderStageFlagBits Stage, const VkShaderModule Module)
{
	VkPipelineShaderStageCreateInfo ShaderStage = {};
	ShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStage.stage = Stage;
	ShaderStage.module = Module;
	ShaderStage.pName = "main";
	ShaderStages.push_back(ShaderStage);
}

VkPipeline FGraphicsPipelineBuilder::BuildPipeline(const TSharedPtr<FGraphicsContext>& GraphicsContext, const TSharedPtr<FGraphicsDevice>& GraphicsDevice) const
{
	VkPipelineViewportStateCreateInfo ViewportState = {};
	ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportState.viewportCount = 1;
	ViewportState.scissorCount = 1;

	VkPipelineColorBlendStateCreateInfo ColorBlendState = {};
	ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlendState.attachmentCount = 1;
	ColorBlendState.pAttachments = &ColorBlendAttachmentState;
	ColorBlendState.logicOpEnable = VK_FALSE;
	ColorBlendState.logicOp = VK_LOGIC_OP_COPY;

	VkPipelineVertexInputStateCreateInfo VertexInputState = {};
	VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	constexpr VkDynamicState DynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_POLYGON_MODE_EXT};
	VkPipelineDynamicStateCreateInfo DynamicState = {};
	DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	DynamicState.dynamicStateCount = _countof(DynamicStates);
	DynamicState.pDynamicStates = DynamicStates;

	VkGraphicsPipelineCreateInfo PipelineCreateInfo = {};
	PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineCreateInfo.pNext = &RenderingCreateInfo;
	PipelineCreateInfo.stageCount = static_cast<uint32_t>(ShaderStages.size());
	PipelineCreateInfo.pStages = ShaderStages.data();
	PipelineCreateInfo.pVertexInputState = &VertexInputState;
	PipelineCreateInfo.pInputAssemblyState = &InputAssemblyState;
	PipelineCreateInfo.pViewportState = &ViewportState;
	PipelineCreateInfo.pRasterizationState = &RasterizationState;
	PipelineCreateInfo.pMultisampleState = &MultisampleState;
	PipelineCreateInfo.pDepthStencilState = &DepthStencilState;
	PipelineCreateInfo.pColorBlendState = &ColorBlendState;
	PipelineCreateInfo.layout = PipelineLayout;
	PipelineCreateInfo.pDynamicState = &DynamicState;

	VkPipeline Pipeline = VK_NULL_HANDLE;
	const FVulkanResult Result = vkCreateGraphicsPipelines(GraphicsDevice->GetHandle(), VK_NULL_HANDLE, 1, &PipelineCreateInfo, GraphicsContext->GetAllocator(),
	                                                       &Pipeline);
	ASSERT(Result == VK_SUCCESS, "Failed to create graphics pipeline");
	return Pipeline;
}
