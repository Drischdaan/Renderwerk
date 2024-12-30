#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

class ENGINE_API FGraphicsDescriptorBuilder
{
public:
	FGraphicsDescriptorBuilder();
	~FGraphicsDescriptorBuilder();

	DEFINE_DEFAULT_COPY_AND_MOVE(FGraphicsDescriptorBuilder);

public:
	void AddBinding(uint32 Binding, VkDescriptorType Type);

	[[nodiscard]] VkDescriptorSetLayout Build(const TSharedPtr<FGraphicsContext>& GraphicsContext, const TSharedPtr<FGraphicsDevice>& GraphicsDevice,
	                                          VkShaderStageFlags ShaderStages);

private:
	TVector<VkDescriptorSetLayoutBinding> Bindings;
};
