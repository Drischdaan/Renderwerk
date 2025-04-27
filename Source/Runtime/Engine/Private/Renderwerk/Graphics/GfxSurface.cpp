#include "pch.hpp"

#include "Renderwerk/Graphics/GfxSurface.hpp"

#include "imgui.h"

#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxFence.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
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

	RenderPasses.push_back(GfxDevice->CreateRenderPass<FTestRenderPass>());
	RenderPasses.push_back(GfxDevice->CreateRenderPass<FGfxImguiRenderPass>());
	for (const TRef<IGfxRenderPass>& RenderPass : RenderPasses)
	{
		RenderPass->CreateResources(GfxDevice->GetResourceManager());
		RenderPassEnableMap.emplace(RenderPass->GetDebugName(), true);
	}

	ResizeDelegateHandle = Window->GetClientResizeDelegate().BindRaw(this, &FGfxSurface::OnWindowResize);
	ImguiDelegateHandle = FGfxImguiRenderPass::GetImguiRenderDelegate().BindRaw(this, &FGfxSurface::OnImguiRender);
}

FGfxSurface::~FGfxSurface()
{
	FGfxImguiRenderPass::GetImguiRenderDelegate().Unbind(ImguiDelegateHandle);
	Window->GetClientResizeDelegate().Unbind(ResizeDelegateHandle);
	FlushWork();

	for (const TRef<IGfxRenderPass>& RenderPass : RenderPasses)
	{
		RenderPass->ReleaseResources(GfxDevice->GetResourceManager());
	}
	RenderPasses.clear();

	PROFILER_DESTROY_RENDER_CONTEXT(ProfilerContext);

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
			for (const TRef<IGfxRenderPass>& RenderPass : RenderPasses)
			{
				if (RenderPassEnableMap.at(RenderPass->GetDebugName()))
				{
					ZoneNamed(RenderPassZone, true);

					FAnsiString Name = FStringUtilities::ConvertToAnsi(RenderPass->GetDebugName());
					RenderPassZone.Name(Name.c_str(), Name.size());
					TracyD3D12ZoneTransient(ProfilerContext, D3D12RenderPassZone,
					                        CommandList->GetNativeObject<ID3D12GraphicsCommandList>(NativeObjectIds::D3D12_CommandList), Name.c_str(), true)

					if (BackBuffer->GetResourceState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
					{
						CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
						CommandList->SetRenderTarget(BackBuffer);
					}
					RenderPass->Render(CommandList, BackBuffer);
				}
			}
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
	for (const TRef<IGfxRenderPass>& RenderPass : RenderPasses)
	{
		RenderPass->ResizeResources(GfxDevice->GetResourceManager(), RequestedResizeWidth, RequestedResizeHeight);
	}
	RW_LOG(Trace, "Resized surface {}x{}", RequestedResizeWidth, RequestedResizeHeight);
	bIsResizedRequested = false;
}

void FGfxSurface::OnImguiRender()
{
	PROFILE_FUNCTION();
	ImGui::Begin("Render Passes");
	{
		for (auto& Entry : RenderPassEnableMap)
		{
			FAnsiString Name = FStringUtilities::ConvertToAnsi(Entry.first);
			ImGui::Checkbox(Name.c_str(), &Entry.second);
		}
	}
	ImGui::End();
}
