#pragma once

#include "RenderPass/TestRenderPass.hpp"

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

class FWindow;

#define RW_DEFAULT_BUFFER_COUNT FORWARD(3)

struct ENGINE_API FGfxFrame
{
	TRef<FGfxCommandList> CommandList;
	TRef<FGfxFence> Fence;
};

class ENGINE_API FGfxSurface : public IGfxDeviceChild
{
public:
	explicit FGfxSurface(FGfxDevice* InGfxDevice, const TRef<FWindow>& InWindow);
	FGfxSurface(FGfxDevice* InGfxDevice, const TRef<FWindow>& InWindow, const FStringView& InDebugName);
	~FGfxSurface() override;

	NON_COPY_MOVEABLE(FGfxSurface)

public:
	void Render();

private:
	[[nodiscard]] FGfxFrame& GetCurrentFrame();
	void AdvanceFrame();

	void FlushWork() const;

	void OnWindowResize(uint32 Width, uint32 Height);
	void ProcessResizeRequest();

private:
	TRef<FWindow> Window;

	TRef<FGfxSwapchain> Swapchain;

	uint8 FrameIndex = 0;
	TVector<FGfxFrame> Frames;

	bool8 bIsResizedRequested = false;
	uint32 RequestedResizeWidth = 0;
	uint32 RequestedResizeHeight = 0;

	FProfilerRenderContext ProfilerContext = nullptr;

	TRef<FTestRenderPass> TestRenderPass;

	FDelegateHandle ResizeDelegateHandle;
};
