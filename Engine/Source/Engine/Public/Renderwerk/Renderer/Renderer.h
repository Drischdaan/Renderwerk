#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsApi.h"

DECLARE_LOG_CHANNEL(LogRenderer);

struct ENGINE_API FRendererDesc
{
	TSharedPtr<FWindow> Window;
};

enum class ENGINE_API ETestStage : uint8
{
	Filled,
	Wireframe,
};

class ENGINE_API FRenderer
{
public:
	FRenderer();
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer);

public:
	void Initialize(const FRendererDesc& InDescription);
	void Destroy();

	void Resize() const;

	void BeginFrame();
	void EndFrame();

private:
	void InitImgui();
	void DrawImgui(VkCommandBuffer CommandBuffer);

private:
	FRendererDesc Description = {};

	TUniquePtr<FGraphicsApi> GraphicsApi;

	VkSurfaceKHR Surface;
	TSharedPtr<FGraphicsDevice> GraphicsDevice;

	TSharedPtr<FGraphicsSwapchain> GraphicsSwapchain;

	FGraphicsFrameId CurrentFrameId = 0;
	TVector<FGraphicsFrame> Frames;

	// TODO: Only temporary, remove this
	VkPipeline TestPipeline;
	VkPipeline TestPipelineWireframe;
	VkPipelineLayout TestPipelineLayout;
	VkDescriptorPool ImguiDescriptorPool;

	ETestStage TestStage = ETestStage::Filled;
};
