#include "pch.hpp"

#include "Renderwerk/Renderer/Swapchain.hpp"

#include "Renderwerk/Platform/Window.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"
#include "Renderwerk/Renderer/Adapter.hpp"
#include "Renderwerk/Renderer/Device.hpp"
#include "Renderwerk/Renderer/GraphicsContext.hpp"

FSwapchain::FSwapchain(FDevice* InDevice, const FSwapchainDesc& InSwapchainDesc)
	: Device(InDevice), SwapchainDesc(InSwapchainDesc)
{
	DXGI_SWAP_CHAIN_DESC1 CreationDesc = {};
	CreationDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	CreationDesc.Width = SwapchainDesc.Window->GetState().WindowWidth;
	CreationDesc.Height = SwapchainDesc.Window->GetState().WindowHeight;
	CreationDesc.Format = SwapchainDesc.Format;
	CreationDesc.BufferCount = SwapchainDesc.BufferCount;
	CreationDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	CreationDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	CreationDesc.SampleDesc.Count = 1;
	CreationDesc.SampleDesc.Quality = 0;
	CreationDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	CreationDesc.Scaling = DXGI_SCALING_NONE;
	CreationDesc.Stereo = false;
	CreationFlags = CreationDesc.Flags;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc = {};
	FullscreenDesc.Windowed = true;

	TObjectHandle<IDXGISwapChain1> TempSwapchain;
	const TObjectHandle<IDXGIFactory7> Factory = Device->GetAdapter()->GetContext()->GetFactory();
	HRESULT Result = Factory->CreateSwapChainForHwnd(Device->GetGraphicsCommandQueue().Get(), SwapchainDesc.Window->GetHandle(), &CreationDesc, &FullscreenDesc, nullptr,
	                                                 &TempSwapchain);
	RW_VERIFY_ID(Result);
	Result = TempSwapchain->QueryInterface(IID_PPV_ARGS(&Swapchain));
	RW_VERIFY_ID(Result);

	AcquireBackBuffers();
}

FSwapchain::~FSwapchain()
{
	ReleaseBackBuffers();
	Swapchain.Reset();
}

uint32 FSwapchain::GetBackBufferIndex() const
{
	return Swapchain->GetCurrentBackBufferIndex();
}

void FSwapchain::Resize(const uint32 NewWidth, const uint32 NewHeight)
{
	ReleaseBackBuffers();
	const HRESULT Result = Swapchain->ResizeBuffers(SwapchainDesc.BufferCount, NewWidth, NewHeight, DXGI_FORMAT_UNKNOWN, CreationFlags);
	RW_VERIFY_ID(Result);
	AcquireBackBuffers();
}

void FSwapchain::Present(const bool8 bUseVSync) const
{
	PROFILE_FUNCTION();
	const HRESULT Result = Swapchain->Present(bUseVSync ? 1 : 0, bUseVSync ? 0 : DXGI_PRESENT_ALLOW_TEARING);
	RW_VERIFY_ID(Result);
}

TObjectHandle<FTexture> FSwapchain::GetBackBuffer(const uint32 Index) const
{
	RW_VERIFY(Index < BackBuffers.size());
	return BackBuffers.at(Index);
}

void FSwapchain::AcquireBackBuffers()
{
	BackBuffers.resize(SwapchainDesc.BufferCount);
	for (uint32 Index = 0; Index < SwapchainDesc.BufferCount; ++Index)
	{
		TObjectHandle<ID3D12Resource2> BackBuffer;
		const HRESULT Result = Swapchain->GetBuffer(Index, IID_PPV_ARGS(&BackBuffer));
		RW_VERIFY_ID(Result);

		FTextureDesc TextureDesc = {};
		TextureDesc.Width = SwapchainDesc.Window->GetState().ClientWidth;
		TextureDesc.Height = SwapchainDesc.Window->GetState().ClientHeight;
		TextureDesc.Format = SwapchainDesc.Format;
		TextureDesc.Usage = ETextureUsage::RenderTarget;
		BackBuffers[Index] = NewObjectHandle<FTexture>(Device, TextureDesc, BackBuffer);
	}
}

void FSwapchain::ReleaseBackBuffers()
{
	BackBuffers.clear();
}
