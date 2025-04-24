#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FTexture;
class FWindow;
class FDevice;

struct ENGINE_API FSwapchainDesc
{
	TRef<FWindow> Window;
	uint32 BufferCount = 3;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
};

class ENGINE_API FSwapchain : public TGraphicsObject<FSwapchain>
{
public:
	FSwapchain(FDevice* InDevice, const FSwapchainDesc& InSwapchainDesc);
	~FSwapchain() override;

	NON_COPY_MOVEABLE(FSwapchain)

public:
	[[nodiscard]] uint32 GetBackBufferIndex() const;

	void Resize(uint32 NewWidth, uint32 NewHeight);

	void Present(bool8 bUseVSync) const;

	[[nodiscard]] TObjectHandle<FTexture> GetBackBuffer(uint32 Index) const;

private:
	void AcquireBackBuffers();
	void ReleaseBackBuffers();

private:
	FDevice* Device;
	FSwapchainDesc SwapchainDesc;

	uint32 CreationFlags;
	TObjectHandle<IDXGISwapChain4> Swapchain;

	TVector<TObjectHandle<FTexture>> BackBuffers;
};
