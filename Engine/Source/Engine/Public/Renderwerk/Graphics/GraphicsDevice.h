#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"

class ENGINE_API FGraphicsDevice
{
public:
	FGraphicsDevice();
	~FGraphicsDevice();

	DEFINE_DEFAULT_COPY_AND_MOVE(FGraphicsDevice);

public:
	void Initialize(const TSharedPtr<FGraphicsContext>& InGraphicsContext, const TSharedPtr<FGraphicsAdapter>& InGraphicsAdapter);
	void Destroy() const;

	void WaitForIdle() const;

public:
	[[nodiscard]] VkDevice GetHandle() const { return Device; }

	[[nodiscard]] TSharedPtr<FGraphicsAdapter> GetAdapter() const { return GraphicsAdapter; }

	[[nodiscard]] VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] VkQueue GetPresentQueue() const { return PresentQueue; }
	[[nodiscard]] VkQueue GetComputeQueue() const { return ComputeQueue; }
	[[nodiscard]] VkQueue GetTransferQueue() const { return TransferQueue; }

private:
	void CreateDevice();
	void AcquireQueues();
	VkQueue GetQueue(const FGraphicsQueueMetadata& Metadata) const;

private:
	TSharedPtr<FGraphicsContext> GraphicsContext = nullptr;
	TSharedPtr<FGraphicsAdapter> GraphicsAdapter = nullptr;

	VkDevice Device = VK_NULL_HANDLE;

	VkQueue GraphicsQueue = VK_NULL_HANDLE;
	VkQueue PresentQueue = VK_NULL_HANDLE;
	VkQueue ComputeQueue = VK_NULL_HANDLE;
	VkQueue TransferQueue = VK_NULL_HANDLE;
};
