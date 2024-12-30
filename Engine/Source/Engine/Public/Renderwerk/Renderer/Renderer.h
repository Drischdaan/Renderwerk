#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsApi.h"
#include "Renderwerk/Scene/Scene.h"

#include <glm/glm.hpp>

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

struct ENGINE_API alignas(16) FVec3
{
	float32 X = 0.f;
	float32 Y = 0.f;
	float32 Z = 0.f;
};

struct ENGINE_API alignas(16) FVec4
{
	float32 X = 0.f;
	float32 Y = 0.f;
	float32 Z = 0.f;
	float32 W = 0.f;
};

struct ENGINE_API FVertex
{
	FVec3 Position;
	FVec4 Color;
};

struct ENGINE_API FDrawPushConstants
{
	glm::mat4 Matrix;
	VkDeviceAddress VertexBufferAddress;
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

	void Resize();

	void BeginFrame();
	void EndFrame();

private:
	void InitImgui();
	void DrawImgui(VkCommandBuffer CommandBuffer);

	void SubmitImmediately(const TFunction<void(VkCommandBuffer)>& Command) const;

	void CreateDepthImage();
	void DestroyDepthImage() const;

private:
	FRendererDesc Description = {};

	TUniquePtr<FGraphicsApi> GraphicsApi;

	VkSurfaceKHR Surface;
	TSharedPtr<FGraphicsDevice> GraphicsDevice;
	TSharedPtr<FGraphicsResourceAllocator> GraphicsResourceAllocator;

	TSharedPtr<FGraphicsSwapchain> GraphicsSwapchain;

	FGraphicsFrameId CurrentFrameId = 0;
	TVector<FGraphicsFrame> Frames;

	// TODO: Only temporary, remove this
	VkPipeline TestPipeline;
	VkPipeline TestPipelineWireframe;
	VkPipelineLayout TestPipelineLayout;
	VkDescriptorPool ImguiDescriptorPool;

	ETestStage TestStage = ETestStage::Filled;

	VkFence ImmediateFence;
	VkCommandPool ImmediateCommandPool;
	VkCommandBuffer ImmediateCommandBuffer;

	VkDescriptorSetLayout DescriptorSetLayout;
	FGraphicsBuffer GpuVertexBuffer;
	FGraphicsBuffer GpuIndexBuffer;

	VkImage DepthImage;
	VkImageView DepthImageView;
	VmaAllocation DepthImageAllocation;
	VkExtent2D DepthImageExtent;
	VkFormat DepthImageFormat;

	float32 FieldOfView = 90.f;
	TSharedPtr<FScene> Scene;
	FEntity TestEntity;
};
