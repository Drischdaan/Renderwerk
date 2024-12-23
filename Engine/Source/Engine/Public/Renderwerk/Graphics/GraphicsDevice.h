#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API FGraphicsDevice
{
public:
	FGraphicsDevice(const TSharedPtr<FGraphicsContext>& InContext, const TSharedPtr<FGraphicsAdapter>& InAdapter);
	~FGraphicsDevice();

	DELETE_COPY_AND_MOVE(FGraphicsDevice);

public:
	void Initialize(const TSpan<const char*>& RequiredExtensions);
	void Destroy() const;

	void WaitForIdle() const;

public:
	[[nodiscard]] VkDevice GetHandle() const { return Context->Device; }

	[[nodiscard]] VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] VkQueue GetPresentQueue() const { return PresentQueue; }
	[[nodiscard]] VkQueue GetComputeQueue() const { return ComputeQueue; }
	[[nodiscard]] VkQueue GetTransferQueue() const { return TransferQueue; }

private:
	[[nodiscard]] VkQueue GetQueue(const FGraphicsQueueMetadata& Metadata) const;

private:
	TSharedPtr<FGraphicsContext> Context;
	TSharedPtr<FGraphicsAdapter> GraphicsAdapter;

	VkQueue GraphicsQueue = VK_NULL_HANDLE;
	VkQueue PresentQueue = VK_NULL_HANDLE;
	VkQueue ComputeQueue = VK_NULL_HANDLE;
	VkQueue TransferQueue = VK_NULL_HANDLE;
};
