#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/VulkanContext.h"

class ENGINE_API FVulkanGraphicsDevice
{
public:
	FVulkanGraphicsDevice(const FVulkanContext& InContext, const TSharedPtr<FVulkanGraphicsAdapter>& InAdapter);
	~FVulkanGraphicsDevice();

	DELETE_COPY_AND_MOVE(FVulkanGraphicsDevice);

public:
	void WaitForIdle() const;

public:
	[[nodiscard]] TSharedPtr<FVulkanGraphicsAdapter> GetAdapter() const { return Adapter; }

	[[nodiscard]] VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] VkQueue GetComputeQueue() const { return ComputeQueue; }
	[[nodiscard]] VkQueue GetTransferQueue() const { return TransferQueue; }
	[[nodiscard]] VkQueue GetPresentQueue() const { return PresentQueue; }

private:
	void CreateDevice();
	void AcquireQueues();

private:
	FVulkanContext Context;
	TSharedPtr<FVulkanGraphicsAdapter> Adapter;

	VkDevice Device;
	VkQueue GraphicsQueue;
	VkQueue ComputeQueue;
	VkQueue TransferQueue;
	VkQueue PresentQueue;

	friend class ENGINE_API FVulkanGraphicsApi;
	friend class ENGINE_API FVulkanGraphicsSwapchain;
	friend class ENGINE_API FVulkanCommandBuffer;
};
