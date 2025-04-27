#include "pch.hpp"

#include "Renderwerk/Graphics/GfxSwapchain.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"
#include "Renderwerk/Platform/Window.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FGfxSwapchain::FGfxSwapchain(FGfxDevice* InGfxDevice, const FGfxSwapchainDesc& InSwapchainDesc)
	: FGfxSwapchain(InGfxDevice, InSwapchainDesc, TEXT("UnnamedSwapchain"))
{
}

FGfxSwapchain::FGfxSwapchain(FGfxDevice* InGfxDevice, const FGfxSwapchainDesc& InSwapchainDesc, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName), SwapchainDesc(InSwapchainDesc)
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

	IDXGIFactory7* Factory = GfxDevice->GetNativeObject<IDXGIFactory7>(NativeObjectIds::DXGI_Factory);
	TComPtr<IDXGISwapChain1> TempSwapchain;
	HRESULT Result = Factory->CreateSwapChainForHwnd(GfxDevice->GetGraphicsQueue().Get(), SwapchainDesc.Window->GetHandle(), &CreationDesc, &FullscreenDesc, nullptr,
	                                                 &TempSwapchain);
	RW_VERIFY_ID(Result);
	Result = TempSwapchain.As(&Swapchain);
	RW_VERIFY_ID(Result);

	AcquireBuffers();
}

FGfxSwapchain::~FGfxSwapchain()
{
	ReleaseBuffers();
	BackBuffers.clear();
	Swapchain.Reset();
}

uint32 FGfxSwapchain::GetBackBufferIndex() const
{
	return Swapchain->GetCurrentBackBufferIndex();
}

void FGfxSwapchain::Resize(const uint32 NewWidth, const uint32 NewHeight)
{
	PROFILE_FUNCTION();
	ReleaseBuffers();
	const HRESULT Result = Swapchain->ResizeBuffers(SwapchainDesc.BufferCount, NewWidth, NewHeight, DXGI_FORMAT_UNKNOWN, CreationFlags);
	RW_VERIFY_ID(Result);
	AcquireBuffers();
}

void FGfxSwapchain::Present(const bool8 bUseVSync) const
{
	PROFILE_FUNCTION();
	const HRESULT Result = Swapchain->Present(bUseVSync ? 1 : 0, bUseVSync ? 0 : DXGI_PRESENT_ALLOW_TEARING);
	RW_VERIFY_ID(Result);
}

TRef<FGfxTexture> FGfxSwapchain::GetBackBuffer(const uint32 Index) const
{
	RW_VERIFY(Index < BackBuffers.size());
	return BackBuffers.at(Index);
}

FNativeObject FGfxSwapchain::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::DXGI_Swapchain)
	{
		return Swapchain.Get();
	}
	return IGfxDeviceChild::GetRawNativeObject(NativeObjectId);
}

void FGfxSwapchain::AcquireBuffers()
{
	BackBuffers.resize(SwapchainDesc.BufferCount);
	for (uint32 Index = 0; Index < SwapchainDesc.BufferCount; ++Index)
	{
		TComPtr<ID3D12Resource2> BackBuffer;
		const HRESULT Result = Swapchain->GetBuffer(Index, IID_PPV_ARGS(&BackBuffer));
		RW_VERIFY_ID(Result);

		FGfxTextureDesc TextureDesc = {};
		TextureDesc.Width = SwapchainDesc.Window->GetState().ClientWidth;
		TextureDesc.Height = SwapchainDesc.Window->GetState().ClientHeight;
		TextureDesc.Format = SwapchainDesc.Format;
		TextureDesc.Usage = EGfxTextureUsage::RenderTarget;
		BackBuffers[Index] = NewRef<FGfxTexture>(GfxDevice, TextureDesc, BackBuffer);
	}
}

void FGfxSwapchain::ReleaseBuffers()
{
	BackBuffers.clear();
}
