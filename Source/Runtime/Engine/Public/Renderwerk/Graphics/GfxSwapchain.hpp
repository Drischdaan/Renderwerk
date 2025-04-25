#pragma once

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

class FWindow;

struct ENGINE_API FGfxSwapchainDesc
{
	TRef<FWindow> Window;
	uint32 BufferCount = 3;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
};

class ENGINE_API FGfxSwapchain : public IGfxDeviceChild
{
public:
	explicit FGfxSwapchain(FGfxDevice* InGfxDevice, const FGfxSwapchainDesc& InSwapchainDesc);
	FGfxSwapchain(FGfxDevice* InGfxDevice, const FGfxSwapchainDesc& InSwapchainDesc, const FStringView& InDebugName);
	~FGfxSwapchain() override;

	NON_COPY_MOVEABLE(FGfxSwapchain)

public:
	[[nodiscard]] uint32 GetBackBufferIndex() const;

	void Resize(uint32 NewWidth, uint32 NewHeight);
	void Present(bool8 bUseVSync = false) const;

	[[nodiscard]] TRef<FGfxTexture> GetBackBuffer(uint32 Index) const;

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

private:
	void AcquireBuffers();
	void ReleaseBuffers();

private:
	FGfxSwapchainDesc SwapchainDesc;

	uint32 CreationFlags;
	TComPtr<IDXGISwapChain4> Swapchain;

	TVector<TRef<FGfxTexture>> BackBuffers;
};
