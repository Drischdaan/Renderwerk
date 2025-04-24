#pragma once

#include "Pipeline/GraphicsPipeline.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"
#include "Renderwerk/Renderer/CommandList.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/GraphicsContext.hpp"
#include "Renderwerk/Renderer/Swapchain.hpp"

#include "tracy/TracyD3D12.hpp"

#define RW_DEFAULT_FRAME_COUNT FORWARD(3)

struct ENGINE_API FGraphicsFrame
{
	TObjectHandle<FCommandList> CommandList;
	TObjectHandle<FFence> Fence;
};

class ENGINE_API FRendererModule : public IEngineModule
{
public:
	FRendererModule();
	~FRendererModule() override = default;

	DEFAULT_COPY_MOVEABLE(FRendererModule)

private:
	void Initialize() override;
	void Shutdown() override;

	void OnTick();

public:
	DEFINE_MODULE_NAME("Renderer")

private:
	TObjectHandle<FGraphicsContext> Context;

	TObjectHandle<FAdapter> Adapter;
	TObjectHandle<FDevice> Device;

	TObjectHandle<FSwapchain> Swapchain;

	uint32 FrameIndex = 0;
	TVector<FGraphicsFrame> Frames;

	TObjectHandle<FGraphicsPipeline> GraphicsPipeline;

	tracy::D3D12QueueCtx* TracyContext = nullptr;

	FDelegateHandle TickDelegateHandle;
};
