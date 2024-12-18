#include "pch.h"

#include "Renderwerk/Graphics/GraphicsDescriptorBuilder.h"

FGraphicsDescriptorBuilder::FGraphicsDescriptorBuilder() = default;

FGraphicsDescriptorBuilder::~FGraphicsDescriptorBuilder() = default;

void FGraphicsDescriptorBuilder::AddBinding(const uint32 Binding, const VkDescriptorType Type)
{
	VkDescriptorSetLayoutBinding BindingInfo;
	BindingInfo.binding = Binding;
	BindingInfo.descriptorType = Type;
	BindingInfo.descriptorCount = 1;
	BindingInfo.stageFlags = VK_SHADER_STAGE_ALL;
	BindingInfo.pImmutableSamplers = nullptr;
	Bindings.push_back(BindingInfo);
}

VkDescriptorSetLayout FGraphicsDescriptorBuilder::Build(const TSharedPtr<FGraphicsContext>& GraphicsContext, const TSharedPtr<FGraphicsDevice>& GraphicsDevice,
                                                        const VkShaderStageFlags ShaderStages)
{
	for (VkDescriptorSetLayoutBinding& Binding : Bindings)
		Binding.stageFlags |= ShaderStages;

	VkDescriptorSetLayoutCreateInfo LayoutInfo = {};
	LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutInfo.bindingCount = static_cast<uint32>(Bindings.size());
	LayoutInfo.pBindings = Bindings.data();

	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	const FVulkanResult Result = vkCreateDescriptorSetLayout(GraphicsDevice->GetHandle(), &LayoutInfo, GraphicsContext->GetAllocator(), &DescriptorSetLayout);
	VERIFY(Result == VK_SUCCESS, "Failed to create descriptor set layout");
	return DescriptorSetLayout;
}
