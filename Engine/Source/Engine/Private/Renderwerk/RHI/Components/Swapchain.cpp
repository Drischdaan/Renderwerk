#include "pch.h"

#include "Renderwerk/RHI/Components/Swapchain.h"

#include "Renderwerk/RHI/RHIContext.h"
#include "Renderwerk/RHI/Commands/CommandQueue.h"
#include "Renderwerk/RHI/Components/Adapter.h"
#include "Renderwerk/RHI/Components/Device.h"
#include "Renderwerk/RHI/Resources/DescriptorHandle.h"
#include "Renderwerk/RHI/Resources/DescriptorHeap.h"

FSwapchain::FSwapchain(FDevice* InDevice, const FSwapchainDesc& InDescription)
	: IDeviceChild(TEXT("Swapchain"), InDevice), Description(InDescription)
{
	FWindowState WindowState = Description.Window->GetState();

	DXGI_SWAP_CHAIN_DESC1 SwapchainDesc;
	SwapchainDesc.Width = WindowState.ClientWidth;
	SwapchainDesc.Height = WindowState.ClientHeight;
	SwapchainDesc.Format = Description.Format;
	SwapchainDesc.Stereo = false;
	SwapchainDesc.SampleDesc.Count = 1;
	SwapchainDesc.SampleDesc.Quality = 0;
	SwapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapchainDesc.BufferCount = Description.BufferCount;
	SwapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	SwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	CreationFlags = SwapchainDesc.Flags;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc = {};
	FullscreenDesc.Windowed = true;

	TComPtr<IDXGISwapChain1> TempSwapchain;
	TComPtr<IDXGIFactory7> Factory = GetDevice()->GetAdapter()->GetContext()->GetFactory();
	ID3D12CommandQueue* GraphicsQueue = GetDevice()->GetGraphicsQueue()->GetHandle().Get();
	FD3DResult CreateResult = Factory->CreateSwapChainForHwnd(GraphicsQueue, Description.Window->GetHandle(), &SwapchainDesc, &FullscreenDesc, nullptr, &TempSwapchain);
	D3D_CHECKM(CreateResult, "Failed to create swapchain");
	D3D_CHECKM(TempSwapchain.As(&Swapchain), "Failed to query swapchain interface");

	FD3DResult AssociateResult = Factory->MakeWindowAssociation(Description.Window->GetHandle(), DXGI_MWA_NO_ALT_ENTER);
	D3D_CHECKM(AssociateResult, "Failed to associate window with swapchain");

	AcquireBackBuffers();
}

FSwapchain::~FSwapchain()
{
	ReleaseBackBuffers();
	Swapchain.Reset();
}

void FSwapchain::Resize(const uint32 Width, const uint32 Height)
{
	RW_PROFILING_MARK_FUNCTION();

	ReleaseBackBuffers();
	FD3DResult ResizeResult = Swapchain->ResizeBuffers(Description.BufferCount, Width, Height, Description.Format, CreationFlags);
	D3D_CHECKM(ResizeResult, "Failed to resize swapchain");
	AcquireBackBuffers();
}

void FSwapchain::Present(const bool8 bUseVSync) const
{
	RW_PROFILING_MARK_FUNCTION();

	uint32 SyncInterval = bUseVSync ? 1 : 0;
	uint32 PresentFlags = bUseVSync ? 0 : DXGI_PRESENT_ALLOW_TEARING;
	FD3DResult PresentResult = Swapchain->Present(SyncInterval, PresentFlags);
	DEBUG_D3D_CHECKM(PresentResult, "Failed to present swapchain");
}

void FSwapchain::SetWindowedFullscreen(const bool8 bWindowedFullscreen) const
{
	Description.Window->SetWindowedFullscreen(Swapchain, bWindowedFullscreen);
}

uint32 FSwapchain::GetImageIndex() const
{
	return Swapchain->GetCurrentBackBufferIndex();
}

TComPtr<ID3D12Resource2> FSwapchain::GetCurrentBackBuffer() const
{
	return GetBackBuffer(GetImageIndex());
}

TComPtr<ID3D12Resource2> FSwapchain::GetBackBuffer(const uint32 Index) const
{
	DEBUG_ASSERTM(Index < Description.BufferCount, "Index out of range");
	return BackBuffers.at(Index);
}

FDescriptorHandle FSwapchain::GetCurrentBackBufferHandle() const
{
	return GetBackBufferHandle(GetImageIndex());
}

FDescriptorHandle FSwapchain::GetBackBufferHandle(const uint32 Index) const
{
	DEBUG_ASSERTM(Index < Description.BufferCount, "Index out of range");
	return BackBufferHandles.at(Index);
}

void FSwapchain::AcquireBackBuffers()
{
	BackBuffers.resize(Description.BufferCount);
	BackBufferHandles.resize(Description.BufferCount);
	for (uint32 Index = 0; Index < Description.BufferCount; ++Index)
	{
		TComPtr<ID3D12Resource2> BackBuffer;
		FD3DResult GetResult = Swapchain->GetBuffer(Index, IID_PPV_ARGS(&BackBuffer));
		D3D_CHECKM(GetResult, "Failed to get back buffer");
		BackBuffers[Index] = BackBuffer;

		FDescriptorHandle BackBufferHandle = GetDevice()->GetRenderTargetViewHeap()->AllocateDescriptor();
		GetDevice()->GetHandle()->CreateRenderTargetView(BackBuffer.Get(), nullptr, BackBufferHandle);
		BackBufferHandles[Index] = BackBufferHandle;
	}
}

void FSwapchain::ReleaseBackBuffers()
{
	for (FDescriptorHandle BackBufferHandle : BackBufferHandles)
		GetDevice()->GetRenderTargetViewHeap()->FreeDescriptor(BackBufferHandle);
	BackBufferHandles.clear();
	for (TComPtr<ID3D12Resource2> BackBuffer : BackBuffers)
		BackBuffer.Reset();
	BackBuffers.clear();
}
