#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/GraphicsPipelineBuilder.h"
#include "Renderwerk/Platform/Filesystem.h"
#include "Renderwerk/Platform/Window.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer() = default;

FRenderer::~FRenderer() = default;

void FRenderer::Initialize(const FRendererDesc& InDescription)
{
	Description = InDescription;
	GraphicsApi = MakeUnique<FGraphicsApi>();
	GraphicsApi->Initialize();

	Surface = GraphicsApi->CreateSurface(Description.Window);

	const TSharedPtr<FGraphicsAdapter> Adapter = GraphicsApi->ChooseCompatibleAdapter(Surface);
	RW_LOG(LogGraphics, Info, "Chosen adapter: {}", Adapter->GetName());
	RW_LOG(LogGraphics, Info, "\t- Type: {}", GetEnumValueName(Adapter->GetType()));
	RW_LOG(LogGraphics, Info, "\t- Vendor: {}", GetVendorString(Adapter->GetVendor()));
	RW_LOG(LogGraphics, Info, "\t- API version: {}.{}.{}", VK_API_VERSION_MAJOR(Adapter->GetApiVersion()), VK_API_VERSION_MINOR(Adapter->GetApiVersion()),
	       VK_API_VERSION_PATCH(Adapter->GetApiVersion()));
	RW_LOG(LogGraphics, Info, "\t- Driver version: {}", Adapter->GetDriverVersionString());
	GraphicsDevice = GraphicsApi->CreateDevice(Adapter);

	FGraphicsSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Surface = Surface;
	GraphicsSwapchain = GraphicsApi->CreateSwapchain(GraphicsDevice);
	GraphicsSwapchain->Initialize(SwapchainDesc);

	Frames.resize(3);
	for (FGraphicsFrame& Frame : Frames)
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.pNext = nullptr;
		CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CommandPoolCreateInfo.queueFamilyIndex = GraphicsDevice->GetAdapter()->GetQueueMetadata(EGraphicsQueueType::Graphics).FamilyIndex;
		FVulkanResult Result = vkCreateCommandPool(GraphicsDevice->GetHandle(), &CommandPoolCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(),
		                                           &Frame.CommandPool);
		VERIFY(Result == VK_SUCCESS, "Failed to create command pool");

		Frame.CommandBuffer = GraphicsApi->AllocateCommandBuffer(GraphicsDevice, Frame.CommandPool);

		VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
		SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		SemaphoreCreateInfo.pNext = nullptr;
		SemaphoreCreateInfo.flags = 0;

		Result = vkCreateSemaphore(GraphicsDevice->GetHandle(), &SemaphoreCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(), &Frame.ImageAvailableSemaphore);
		VERIFY(Result == VK_SUCCESS, "Failed to create image available semaphore");

		Result = vkCreateSemaphore(GraphicsDevice->GetHandle(), &SemaphoreCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(), &Frame.RenderFinishedSemaphore);
		VERIFY(Result == VK_SUCCESS, "Failed to create render finished semaphore");

		VkFenceCreateInfo FenceCreateInfo = {};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = nullptr;
		FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		Result = vkCreateFence(GraphicsDevice->GetHandle(), &FenceCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(), &Frame.InFlightFence);
		VERIFY(Result == VK_SUCCESS, "Failed to create in-flight fence");
	}

	// TODO: Only temporary, remove this
	{
		FFile VertexShaderFile = FFile("Assets/Shaders/DefaultVert.spv");
		VkShaderModule VertexShaderModule = GraphicsApi->CreateShaderModule(GraphicsDevice, VertexShaderFile.Read());

		FFile FragmentShaderFile = FFile("Assets/Shaders/DefaultFrag.spv");
		VkShaderModule FragmentShaderModule = GraphicsApi->CreateShaderModule(GraphicsDevice, FragmentShaderFile.Read());

		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo{};
		PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutCreateInfo.pNext = nullptr;
		PipelineLayoutCreateInfo.flags = 0;
		PipelineLayoutCreateInfo.setLayoutCount = 0;
		PipelineLayoutCreateInfo.pSetLayouts = nullptr;
		PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
		FVulkanResult Result = vkCreatePipelineLayout(GraphicsDevice->GetHandle(), &PipelineLayoutCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(),
		                                              &TestPipelineLayout);
		ASSERT(Result == VK_SUCCESS, "Failed to create pipeline layout");

		FGraphicsPipelineBuilder PipelineBuilder = {};
		PipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		PipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
		PipelineBuilder.SetCullMode(VK_CULL_MODE_NONE);
		PipelineBuilder.SetPipelineLayout(TestPipelineLayout);
		PipelineBuilder.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, VertexShaderModule);
		PipelineBuilder.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, FragmentShaderModule);
		TestPipeline = PipelineBuilder.BuildPipeline(GraphicsApi->GetGraphicsContext(), GraphicsDevice);

		vkDestroyShaderModule(GraphicsDevice->GetHandle(), VertexShaderModule, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroyShaderModule(GraphicsDevice->GetHandle(), FragmentShaderModule, GraphicsApi->GetGraphicsContext()->GetAllocator());
	}
}

void FRenderer::Destroy()
{
	GraphicsDevice->WaitForIdle();

	// TODO: Only temporary, remove this
	{
		vkDestroyPipelineLayout(GraphicsDevice->GetHandle(), TestPipelineLayout, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroyPipeline(GraphicsDevice->GetHandle(), TestPipeline, GraphicsApi->GetGraphicsContext()->GetAllocator());
	}

	for (FGraphicsFrame& Frame : Frames)
	{
		vkDestroyFence(GraphicsDevice->GetHandle(), Frame.InFlightFence, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroySemaphore(GraphicsDevice->GetHandle(), Frame.RenderFinishedSemaphore, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroySemaphore(GraphicsDevice->GetHandle(), Frame.ImageAvailableSemaphore, GraphicsApi->GetGraphicsContext()->GetAllocator());
		GraphicsApi->DeallocateCommandBuffer(GraphicsDevice, Frame.CommandPool, Frame.CommandBuffer);
		Frame.CommandBuffer.reset();
		vkDestroyCommandPool(GraphicsDevice->GetHandle(), Frame.CommandPool, GraphicsApi->GetGraphicsContext()->GetAllocator());
	}
	GraphicsSwapchain->Destroy();
	GraphicsSwapchain.reset();
	GraphicsDevice->Destroy();
	GraphicsDevice.reset();
	GraphicsApi->DestroySurface(Surface);
	GraphicsApi->Destroy();
	GraphicsApi.reset();
}

void FRenderer::Resize() const
{
	PROFILE_FUNCTION();
	GraphicsDevice->WaitForIdle();

	GraphicsSwapchain->Resize();
}

void FRenderer::BeginFrame()
{
	PROFILE_FUNCTION();
	FGraphicsFrame& Frame = Frames.at(CurrentFrameId);

	{
		PROFILE_SCOPE("vkWaitForFences");
		FVulkanResult Result = vkWaitForFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence, VK_TRUE, UINT64_MAX);
		VERIFY(Result == VK_SUCCESS, "Failed to wait for in-flight fence");
		Result = vkResetFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence);
		VERIFY(Result == VK_SUCCESS, "Failed to reset in-flight fence");
	}

	Frame.DeletionQueue.Process();

	if (!GraphicsSwapchain->AcquireNextImageIndex(Frame.ImageAvailableSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
	}
	Frame.ImageIndex = GraphicsSwapchain->GetCurrentImageIndex();

	const TSharedPtr<FGraphicsCommandBuffer> CommandBuffer = Frame.CommandBuffer;
	const FGraphicsBackBuffer BackBuffer = GraphicsSwapchain->GetBackBuffer(Frame.ImageIndex);

	CommandBuffer->Reset();
	CommandBuffer->Begin();

	CommandBuffer->TransitionImageLayout(BackBuffer.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	CommandBuffer->BeginDebugLabel("ClearBackBuffer");
	CommandBuffer->ClearImage(BackBuffer.Image, VK_IMAGE_LAYOUT_GENERAL, {0.1f, 0.1f, 0.1f, 1.0f});
	CommandBuffer->EndDebugLabel();

	// TODO: Only temporary, remove this
	{
		CommandBuffer->BeginDebugLabel("DrawTriangle");
		VkRenderingAttachmentInfo ColorAttachmentInfo = {};
		ColorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		ColorAttachmentInfo.pNext = nullptr;
		ColorAttachmentInfo.imageView = BackBuffer.ImageView;
		ColorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		ColorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		ColorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo RenderingInfo = {};
		RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		RenderingInfo.renderArea.offset = {.x = 0, .y = 0};
		RenderingInfo.renderArea.extent = GraphicsSwapchain->GetExtent();
		RenderingInfo.layerCount = 1;
		RenderingInfo.colorAttachmentCount = 1;
		RenderingInfo.pColorAttachments = &ColorAttachmentInfo;
		vkCmdBeginRendering(CommandBuffer->GetHandle(), &RenderingInfo);

		vkCmdBindPipeline(CommandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, TestPipeline);

		VkViewport Viewport;
		Viewport.x = 0;
		Viewport.y = 0;
		Viewport.width = static_cast<float32>(GraphicsSwapchain->GetExtent().width);
		Viewport.height = static_cast<float32>(GraphicsSwapchain->GetExtent().height);
		Viewport.minDepth = 0.f;
		Viewport.maxDepth = 1.f;
		vkCmdSetViewport(CommandBuffer->GetHandle(), 0, 1, &Viewport);

		VkRect2D Scissor;
		Scissor.offset.x = 0;
		Scissor.offset.y = 0;
		Scissor.extent.width = GraphicsSwapchain->GetExtent().width;
		Scissor.extent.height = GraphicsSwapchain->GetExtent().height;
		vkCmdSetScissor(CommandBuffer->GetHandle(), 0, 1, &Scissor);

		vkCmdDraw(CommandBuffer->GetHandle(), 3, 1, 0, 0);
		vkCmdEndRendering(CommandBuffer->GetHandle());
		CommandBuffer->EndDebugLabel();
	}
}

void FRenderer::EndFrame()
{
	PROFILE_FUNCTION();
	const FGraphicsFrame& Frame = Frames.at(CurrentFrameId);
	const TSharedPtr<FGraphicsCommandBuffer> CommandBuffer = Frame.CommandBuffer;
	const FGraphicsBackBuffer BackBuffer = GraphicsSwapchain->GetBackBuffer(Frame.ImageIndex);

	CommandBuffer->TransitionImageLayout(BackBuffer.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	CommandBuffer->End();

	GraphicsApi->SubmitQueue(GraphicsDevice->GetGraphicsQueue(), Frame);

	if (!GraphicsSwapchain->Present(Frame.RenderFinishedSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
		return;
	}
	CurrentFrameId = (CurrentFrameId + 1) % Frames.size();
}
