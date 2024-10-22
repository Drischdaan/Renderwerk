#pragma once

#include "Renderwerk/RHI/RHICommon.h"

struct RENDERWERK_API FSwapchainDesc
{
	TSharedPtr<FWindow> Window;
	uint32 BufferCount = 3;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
};

class RENDERWERK_API FSwapchain : public IDeviceChild
{
public:
	FSwapchain(FDevice* InDevice, const FSwapchainDesc& InDescription);
	~FSwapchain() override;

	DELETE_COPY_AND_MOVE(FSwapchain);

public:
	void Resize(uint32 Width, uint32 Height);
	void Present(bool8 bUseVSync = true) const;

	void SetWindowedFullscreen(bool8 bWindowedFullscreen) const;

	NODISCARD uint32 GetImageIndex() const;

	NODISCARD TComPtr<ID3D12Resource2> GetCurrentBackBuffer() const;
	NODISCARD TComPtr<ID3D12Resource2> GetBackBuffer(uint32 Index) const;

	NODISCARD FDescriptorHandle GetCurrentBackBufferHandle() const;
	NODISCARD FDescriptorHandle GetBackBufferHandle(uint32 Index) const;

public:
	NODISCARD TComPtr<IDXGISwapChain4> GetHandle() const { return Swapchain; }

private:
	void AcquireBackBuffers();
	void ReleaseBackBuffers();

private:
	FSwapchainDesc Description;

	uint32 CreationFlags;
	TComPtr<IDXGISwapChain4> Swapchain;

	TVector<TComPtr<ID3D12Resource2>> BackBuffers;
	TVector<FDescriptorHandle> BackBufferHandles;
};
