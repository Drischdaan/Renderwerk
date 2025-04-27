#include "pch.hpp"

#include "Renderwerk/Graphics/GfxSurface.hpp"

#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxFence.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
#include "Renderwerk/Graphics/GfxShaderCompiler.hpp"
#include "Renderwerk/Graphics/GfxSwapchain.hpp"
#include "Renderwerk/Graphics/Pipeline/GfxGraphicsPipeline.hpp"
#include "Renderwerk/Platform/Window.hpp"
#include "Renderwerk/Platform/Threading/ScopedLock.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FGfxSurface::FGfxSurface(FGfxDevice* InGfxDevice, const TRef<FWindow>& InWindow)
	: FGfxSurface(InGfxDevice, InWindow, TEXT("UnnamedSurface"))
{
}

FGfxSurface::FGfxSurface(FGfxDevice* InGfxDevice, const TRef<FWindow>& InWindow, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName), Window(InWindow)
{
	FGfxSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Window = Window;
	SwapchainDesc.BufferCount = RW_DEFAULT_BUFFER_COUNT;
	Swapchain = GfxDevice->CreateSwapchain(SwapchainDesc);

	Frames.resize(RW_DEFAULT_BUFFER_COUNT);
	for (FGfxFrame& Frame : Frames)
	{
		Frame.CommandList = GfxDevice->CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
		Frame.Fence = GfxDevice->CreateFence();
	}

	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	ProfilerContext = PROFILER_RENDER_CONTEXT(NativeDevice, GfxDevice->GetGraphicsQueue().Get());

	FGfxTextureDesc TextureDesc = {};
	TextureDesc.Width = 16;
	TextureDesc.Height = 16;
	TextureDesc.Usage = EGfxTextureUsage::SharedResource;
	TestTexture = GfxDevice->GetResourceManager()->AllocateTexture(TextureDesc);

	const TVector<uint8> TextureData = {
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
		255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
	};
	TestTexture->SetData(TextureData);

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

	const FGfxShaderModule VertexShader = GfxDevice->GetShaderCompiler()->CompilerShader(EGfxShaderType::Vertex, ShaderSource, L"VSMain");
	const FGfxShaderModule PixelShader = GfxDevice->GetShaderCompiler()->CompilerShader(EGfxShaderType::Pixel, ShaderSource, L"PSMain");

	FGfxGraphicsPipelineDesc GraphicsPipelineDesc = {};
	GraphicsPipelineDesc.Shaders[EGfxShaderType::Vertex] = VertexShader;
	GraphicsPipelineDesc.Shaders[EGfxShaderType::Pixel] = PixelShader;
	GraphicsPipelineDesc.Formats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
	GraphicsPipelineDesc.RootSignature = GfxDevice->CreateRootSignature();
	GraphicsPipeline = NewRef<FGfxGraphicsPipeline>(GfxDevice, GraphicsPipelineDesc);

	ResizeDelegateHandle = Window->GetClientResizeDelegate().BindRaw(this, &FGfxSurface::OnWindowResize);
}

FGfxSurface::~FGfxSurface()
{
	Window->GetClientResizeDelegate().Unbind(ResizeDelegateHandle);
	FlushWork();

	PROFILER_DESTROY_RENDER_CONTEXT(ProfilerContext);

	GraphicsPipeline.reset();
	TestTexture.reset();

	Frames.clear();
	Swapchain.reset();
	Window.reset();
}

void FGfxSurface::Render()
{
	PROFILE_FUNCTION();
	if (Window->GetState().bIsMinimized)
	{
		return;
	}

	if (bIsResizedRequested)
	{
		ProcessResizeRequest();
	}

	PROFILE_RENDER_FRAME(ProfilerContext);

	const FGfxFrame& Frame = GetCurrentFrame();
	RW_VERIFY(Frame.Fence->Wait());

	GfxDevice->GetResourceManager()->ReleaseUploadRequests();

	const TRef<FGfxTexture>& BackBuffer = Swapchain->GetBackBuffer(Swapchain->GetBackBufferIndex());

	const TRef<FGfxCommandList> CommandList = Frame.CommandList;
	CommandList->Open();
	{
		PROFILE_RENDER_SCOPE(ProfilerContext, CommandList, "Frame")

		{
			PROFILE_RENDER_SCOPE(ProfilerContext, CommandList, "ResourceUploading")
			GfxDevice->GetResourceManager()->CollectResourceUploads();
			GfxDevice->GetResourceManager()->FlushUploadRequests(CommandList);
		}

		CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
		CommandList->SetRenderTarget(BackBuffer);

		constexpr float32 Color[4] = {0.1f, 0.1f, 0.1f, 1.0f};
		CommandList->ClearRenderTarget(BackBuffer, Color);

		{
			CommandList->SetPipeline(GraphicsPipeline);
			CommandList->SetTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			CommandList->SetViewport(Window->GetState().ClientWidth, Window->GetState().ClientHeight);
			CommandList->SetScissor(Window->GetState().ClientWidth, Window->GetState().ClientHeight);

			CommandList->DrawInstanced(3, 1);
		}

		CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_PRESENT);
	}
	CommandList->Close();

	GfxDevice->SubmitGraphicsWork(CommandList);
	Swapchain->Present(false);
	Frame.Fence->SignalCommandQueue(GfxDevice->GetGraphicsQueue());

	AdvanceFrame();
}

FGfxFrame& FGfxSurface::GetCurrentFrame()
{
	return Frames.at(FrameIndex);
}

void FGfxSurface::AdvanceFrame()
{
	FrameIndex = (FrameIndex + 1) % RW_DEFAULT_BUFFER_COUNT;
}

void FGfxSurface::FlushWork() const
{
	PROFILE_FUNCTION();
	for (uint32 Index = 0; Index < RW_DEFAULT_BUFFER_COUNT; ++Index)
	{
		GfxDevice->FlushGraphicsQueue();
	}
}

void FGfxSurface::OnWindowResize(const uint32 Width, const uint32 Height)
{
	PROFILE_FUNCTION();
	if (Width == 0 || Height == 0)
	{
		RW_LOG(Trace, "Surface wasn't resized. Invalid width or height");
		return;
	}
	bIsResizedRequested = true;
	RequestedResizeWidth = Width;
	RequestedResizeHeight = Height;
}

void FGfxSurface::ProcessResizeRequest()
{
	FlushWork();
	Swapchain->Resize(RequestedResizeWidth, RequestedResizeHeight);
	RW_LOG(Trace, "Resized surface {}x{}", RequestedResizeWidth, RequestedResizeHeight);
	bIsResizedRequested = false;
}
