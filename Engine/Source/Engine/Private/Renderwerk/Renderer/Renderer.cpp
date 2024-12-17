#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "imgui.h"

#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_win32.h"

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

		InitImgui();
	}
}

void FRenderer::Destroy()
{
	GraphicsDevice->WaitForIdle();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplWin32_Shutdown();
	vkDestroyDescriptorPool(GraphicsDevice->GetHandle(), ImguiDescriptorPool, GraphicsApi->GetGraphicsContext()->GetAllocator());
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

		CommandBuffer->BeginDebugLabel("DrawTriangle");
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

		vkCmdSetPolygonModeEXT(CommandBuffer->GetHandle(), TestStage == ETestStage::Filled ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE);

		vkCmdDraw(CommandBuffer->GetHandle(), 3, 1, 0, 0);
		CommandBuffer->EndDebugLabel();

		CommandBuffer->BeginDebugLabel("DrawImgui");
		DrawImgui(CommandBuffer->GetHandle());
		CommandBuffer->EndDebugLabel();

		vkCmdEndRendering(CommandBuffer->GetHandle());
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

void FRenderer::InitImgui()
{
	VkDescriptorPoolSize DescriptorPoolSizes[] = {
		{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
	};

	VkDescriptorPoolCreateInfo PoolCreateInfo = {};
	PoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	PoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	PoolCreateInfo.maxSets = 1000;
	PoolCreateInfo.poolSizeCount = _countof(DescriptorPoolSizes);
	PoolCreateInfo.pPoolSizes = DescriptorPoolSizes;

	const FVulkanResult Result = vkCreateDescriptorPool(GraphicsDevice->GetHandle(), &PoolCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(),
	                                                    &ImguiDescriptorPool);
	VERIFY(Result == VK_SUCCESS, "Failed to create descriptor pool");

	ImGui::CreateContext();

	ImGui_ImplWin32_Init(Description.Window->GetHandle());

	const VkFormat ColorFormat = GraphicsSwapchain->GetFormat();
	ImGui_ImplVulkan_InitInfo VulkanInitInfo = {};
	VulkanInitInfo.Instance = GraphicsApi->GetGraphicsContext()->GetInstance();
	VulkanInitInfo.PhysicalDevice = GraphicsDevice->GetAdapter()->GetHandle();
	VulkanInitInfo.Device = GraphicsDevice->GetHandle();
	VulkanInitInfo.Queue = GraphicsDevice->GetGraphicsQueue();
	VulkanInitInfo.DescriptorPool = ImguiDescriptorPool;
	VulkanInitInfo.MinImageCount = 3;
	VulkanInitInfo.ImageCount = 3;
	VulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	VulkanInitInfo.UseDynamicRendering = true;
	VulkanInitInfo.PipelineRenderingCreateInfo = {};
	VulkanInitInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	VulkanInitInfo.PipelineRenderingCreateInfo.pNext = nullptr;
	VulkanInitInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	VulkanInitInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &ColorFormat;

	ImGui_ImplVulkan_Init(&VulkanInitInfo);
	ImGui_ImplVulkan_CreateFontsTexture();

	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->WindowPadding = ImVec2(15, 15);
	Style->WindowRounding = 5.0f;
	Style->FramePadding = ImVec2(5, 5);
	Style->FrameRounding = 4.0f;
	Style->ItemSpacing = ImVec2(12, 8);
	Style->ItemInnerSpacing = ImVec2(8, 6);
	Style->IndentSpacing = 25.0f;
	Style->ScrollbarSize = 15.0f;
	Style->ScrollbarRounding = 9.0f;
	Style->GrabMinSize = 5.0f;
	Style->GrabRounding = 3.0f;

	Style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	Style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	Style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	Style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	Style->Colors[ImGuiCol_Border] = ImVec4(0.40f, 0.40f, 0.43f, 0.48f);
	Style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	Style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	Style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	Style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	Style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	Style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	Style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	Style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	Style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	Style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	Style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	Style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	Style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	Style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	Style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	Style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	Style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	Style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	Style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	Style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	Style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	Style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	Style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	Style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	Style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	Style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	Style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	Style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	Style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
}

void FRenderer::DrawImgui(const VkCommandBuffer CommandBuffer)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");

	constexpr auto Stages = GetEnumValueNamePair<ETestStage>();
	if (ImGui::BeginCombo("Fill Mode", GetEnumValueName(TestStage).data()))
	{
		for (const auto& Stage : Stages)
		{
			const bool8 bIsSelected = TestStage == Stage.first;
			if (ImGui::Selectable(Stage.second.data(), bIsSelected))
				TestStage = Stage.first;
			if (bIsSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CommandBuffer);
}
