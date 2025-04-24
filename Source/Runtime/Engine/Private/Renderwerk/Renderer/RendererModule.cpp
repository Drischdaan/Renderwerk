#include "pch.hpp"

#include "Renderwerk/Renderer/RendererModule.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Platform/Window.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"
#include "Renderwerk/Renderer/RootSignature.hpp"
#include "Renderwerk/Renderer/ShaderCompiler.hpp"

FRendererModule::FRendererModule()
	: IEngineModule(EEngineThreadAffinity::Render)
{
}

void FRendererModule::Initialize()
{
	PROFILE_FUNCTION();

	FGraphicsContextDesc ContextDesc = {
#ifdef RW_CONFIG_DEBUG
		.bEnableDebugLayer = true,
		.bEnableGPUValidation = true,
#else
		.bEnableDebugLayer = false,
		.bEnableGPUValidation = false,
#endif
	};
	Context = NewObjectHandle<FGraphicsContext>(ContextDesc);

	Adapter = Context->GetSuitableAdapter();
	RW_VERIFY_MSG(Adapter, "Unable to find suitable adapter");
	RW_LOG(Info, "Using adapter:");
	RW_LOG(Info, "\t- Name: {}", Adapter->GetName());
	RW_LOG(Info, "\t- Vendor: {}", FAdapter::GetVendorString(Adapter->GetVendor()));
	RW_LOG(Info, "\t- VideoMemory: {}", FStringUtilities::FormatMemorySize(Adapter->GetDedicatedVideoMemory()));
	RW_LOG(Info, "\t- SystemMemory: {}", FStringUtilities::FormatMemorySize(Adapter->GetSharedSystemMemory()));
	RW_LOG(Info, "\t- FeatureLevel: {}", D3DUtility::ToString(Adapter->GetFeatureLevel()));
	RW_LOG(Info, "\t- ShaderModel: {}", D3DUtility::ToString(Adapter->GetShaderModel()));
	RW_LOG(Info, "\t- ResourceBinding: {}", D3DUtility::ToString(Adapter->GetResourceBindingTier()));
	RW_LOG(Info, "\t- Raytracing: {}", D3DUtility::ToString(Adapter->GetRaytracingTier()));
	RW_LOG(Info, "\t- MeshShader: {}", D3DUtility::ToString(Adapter->GetMeshShaderTier()));

	RW_LOG(Info, "Adapter Memory Stats:");
	{
		const FAdapterMemoryStats DedicatedStats = Adapter->QueryMemoryStats(EAdapterMemoryType::Dedicated);
		RW_LOG(Info, "\t- Dedicated:");
		RW_LOG(Info, "\t\t- Available: {}", FStringUtilities::FormatMemorySize(DedicatedStats.AvailableMemory));
		RW_LOG(Info, "\t\t- Usage: {}", FStringUtilities::FormatMemorySize(DedicatedStats.UsedMemory));

		const FAdapterMemoryStats SharedStats = Adapter->QueryMemoryStats(EAdapterMemoryType::Shared);
		RW_LOG(Info, "\t- Shared:");
		RW_LOG(Info, "\t\t- Available: {}", FStringUtilities::FormatMemorySize(SharedStats.AvailableMemory));
		RW_LOG(Info, "\t\t- Usage: {}", FStringUtilities::FormatMemorySize(SharedStats.UsedMemory));
	}

	constexpr FDeviceDesc DeviceDesc = {
#ifdef RW_CONFIG_DEBUG
		.bEnableInfoQueue = true,
#else
		.bEnableInfoQueue = false,
#endif
	};
	Device = Adapter->CreateDevice(DeviceDesc);

	FSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Window = GetEngine()->GetWindow();
	Swapchain = NewObjectHandle<FSwapchain>(Device.Get(), SwapchainDesc);

	Frames.resize(RW_DEFAULT_FRAME_COUNT);
	for (uint32 Index = 0; Index < RW_DEFAULT_FRAME_COUNT; ++Index)
	{
		Frames[Index].CommandList = NewObjectHandle<FCommandList>(Device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
		Frames[Index].Fence = Device->CreateFence();
	}

	const FAnsiString ShaderSource = R"(#pragma vertex VSMain
#pragma pixel PSMain

static const float3 VERTICES[] = {
    float3(-0.5, -0.5, 0.0),
    float3( 0.5, -0.5, 0.0),
    float3( 0.0,  0.5, 0.0)
};

static const float3 COLORS[] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

struct VertexOut
{
    float4 vPosition : SV_POSITION;
    float3 vColor : COLOR;
};

VertexOut VSMain(uint nVertexID : SV_VertexID)
{
    VertexOut Out = (VertexOut)0;

    Out.vPosition = float4(VERTICES[nVertexID], 1.0);
    Out.vColor = COLORS[nVertexID];

    return Out;
}

float4 PSMain(VertexOut Out) : SV_Target
{
    return float4(Out.vColor, 1.0);
})";

	const FShaderModule VertexShader = Device->GetShaderCompiler()->CompilerShader(EShaderType::Vertex, ShaderSource, L"VSMain");
	const FShaderModule PixelShader = Device->GetShaderCompiler()->CompilerShader(EShaderType::Pixel, ShaderSource, L"PSMain");

	FGraphicsPipelineDesc GraphicsPipelineDesc = {};
	GraphicsPipelineDesc.Shaders[EShaderType::Vertex] = VertexShader;
	GraphicsPipelineDesc.Shaders[EShaderType::Pixel] = PixelShader;
	GraphicsPipelineDesc.Formats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
	GraphicsPipelineDesc.RootSignature = NewObjectHandle<FRootSignature>(Device.Get());
	GraphicsPipeline = NewObjectHandle<FGraphicsPipeline>(Device.Get(), GraphicsPipelineDesc);

	TracyContext = TracyD3D12Context(Device->GetHandle(), Device->GetGraphicsCommandQueue())

	TickDelegateHandle = GetEngine()->GetRenderThreadTickDelegate().BindRaw(this, &FRendererModule::OnTick);

	RW_LOG(Info, "Renderer module initialized");
}

void FRendererModule::Shutdown()
{
	GetEngine()->GetRenderThreadTickDelegate().Unbind(TickDelegateHandle);

	TracyD3D12Destroy(TracyContext)
	TracyContext = nullptr;

	for (size64 Index = 0; Index < Frames.size(); ++Index)
	{
		Device->FlushWork();
	}

	GraphicsPipeline.Reset();
	Frames.clear();

	Swapchain.Reset();

	Device.Reset();
	Adapter.Reset();
	Context.Reset();
	RW_LOG(Info, "Renderer module shutdown");
}

void FRendererModule::OnTick()
{
	const uint32 BackBufferIndex = Swapchain->GetBackBufferIndex();
	const FGraphicsFrame& Frame = Frames.at(FrameIndex);
	RW_VERIFY(Frame.Fence->Wait());

	const TObjectHandle<FCommandList> CommandList = Frame.CommandList;
	{
		PROFILE_SCOPE("CommandListRecording");
		TracyD3D12NewFrame(TracyContext)

		const TObjectHandle<FTexture> BackBuffer = Swapchain->GetBackBuffer(BackBufferIndex);

		CommandList->Open();
		{
			TracyD3D12Zone(TracyContext, CommandList->GetHandle(), "Frame")
			{
				TracyD3D12Zone(TracyContext, CommandList->GetHandle(), "ClearTargets")

				CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
				constexpr float32 Colors[4] = {1.0f, 0.5f, 0.5f, 1.0f};
				CommandList->ClearRenderTarget(BackBuffer, Colors);
				CommandList->SetRenderTarget(BackBuffer);
			}

			{
				TracyD3D12Zone(TracyContext, CommandList->GetHandle(), "DrawTriangle")

				CommandList->GetHandle()->SetPipelineState(GraphicsPipeline->GetHandle());
				CommandList->GetHandle()->SetGraphicsRootSignature(GraphicsPipeline->GetRootSignature()->GetHandle());
				CommandList->GetHandle()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				D3D12_VIEWPORT Viewport;
				Viewport.Width = static_cast<float32>(GetEngine()->GetWindow()->GetState().ClientWidth);
				Viewport.Height = static_cast<float32>(GetEngine()->GetWindow()->GetState().ClientHeight);
				Viewport.MinDepth = 0.0f;
				Viewport.MaxDepth = 1.0f;
				Viewport.TopLeftX = 0;
				Viewport.TopLeftY = 0;

				D3D12_RECT Rect;
				Rect.right = static_cast<LONG>(Viewport.Width);
				Rect.bottom = static_cast<LONG>(Viewport.Height);
				Rect.top = 0;
				Rect.left = 0;

				CommandList->GetHandle()->RSSetViewports(1, &Viewport);
				CommandList->GetHandle()->RSSetScissorRects(1, &Rect);

				CommandList->GetHandle()->DrawInstanced(3, 1, 0, 0);
			}

			CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_PRESENT);
		}
		CommandList->Close();
	}
	Device->SubmitGraphicsWork(CommandList);
	Swapchain->Present(false);
	Device->SignalGraphicsQueue(Frame.Fence);
	FrameIndex = (FrameIndex + 1) % RW_DEFAULT_FRAME_COUNT;
}
