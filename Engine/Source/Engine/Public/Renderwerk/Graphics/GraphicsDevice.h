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
	void Destroy();

	void WaitForIdle() const;

public:
	[[nodiscard]] VkDevice GetHandle() const { return Context->Device; }

	[[nodiscard]] TSharedPtr<FGraphicsCommandQueue> GetGraphicsQueue() const { return GraphicsCommandQueue; }
	[[nodiscard]] TSharedPtr<FGraphicsCommandQueue> GetPresentQueue() const { return PresentCommandQueue; }

private:
	[[nodiscard]] TSharedPtr<FGraphicsCommandQueue> CreateQueue(EGraphicsQueueType Type) const;

private:
	TSharedPtr<FGraphicsContext> Context;
	TSharedPtr<FGraphicsAdapter> GraphicsAdapter;

	TSharedPtr<FGraphicsCommandQueue> GraphicsCommandQueue;
	TSharedPtr<FGraphicsCommandQueue> PresentCommandQueue;
	TSharedPtr<FGraphicsCommandQueue> ComputeCommandQueue;
	TSharedPtr<FGraphicsCommandQueue> TransferCommandQueue;
};
