#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API FGraphicsCommandBuffer
{
public:
	FGraphicsCommandBuffer(const VkCommandBuffer& InCommandBuffer);
	~FGraphicsCommandBuffer();

	DELETE_COPY_AND_MOVE(FGraphicsCommandBuffer);

public:
	void Reset() const;
	void Begin() const;
	void End() const;

	void TransitionImageLayout(const VkImage& Image, VkImageLayout OldLayout, VkImageLayout NewLayout) const;

	void ClearImage(const VkImage& Image, VkImageLayout Layout, const VkClearColorValue& ClearColor) const;

	void BeginDebugLabel(const FString& Name, const VkClearColorValue& Color = {1.0f, 1.0f, 1.0f, 1.0f}) const;
	void EndDebugLabel() const;

public:
	[[nodiscard]] VkCommandBuffer GetHandle() const { return CommandBuffer; }

private:
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
};
