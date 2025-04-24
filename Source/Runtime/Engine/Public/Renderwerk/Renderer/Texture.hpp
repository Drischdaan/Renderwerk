#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Renderer/DescriptorHeap.hpp"
#include "Renderwerk/Renderer/Resource.hpp"

enum class ENGINE_API ETextureUsage : uint32
{
	RenderTarget = BIT(1),
	DepthTarget = BIT(2),
	Storage = BIT(3),
	SharedResource = BIT(4),
};

DEFINE_ENUM_FLAGS(ETextureUsage)

enum class ENGINE_API ETextureDimension : uint8
{
	Texture1D = 0,
	Texture2D,
	Texture3D,
};

struct ENGINE_API FTextureDesc
{
	ETextureUsage Usage;
	uint32 Width = 0;
	uint32 Height = 0;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uint16 MipLevels = 0;
	ETextureDimension Dimension = ETextureDimension::Texture2D;
	D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_COMMON;
	FStringView DebugName;
};

class ENGINE_API FTexture : public IResource
{
public:
	FTexture(FDevice* InDevice, const FTextureDesc& InTextureDesc, const TObjectHandle<ID3D12Resource2>& InResource);
	FTexture(FDevice* InDevice, const FTextureDesc& InTextureDesc);
	~FTexture() override;

	NON_COPY_MOVEABLE(FTexture)

public:
	[[nodiscard]] uint32 GetWidth() const;
	[[nodiscard]] uint32 GetHeight() const;
	[[nodiscard]] uint32 GetMipLevels() const;
	[[nodiscard]] ETextureUsage GetUsage() const;
	[[nodiscard]] DXGI_FORMAT GetFormat() const;

private:
	void AllocateRenderTarget();

public:
	[[nodiscard]] FDescriptorHandle GetRTVDescriptorHandle() const { return RTVDescriptorHandle; }

private:
	FTextureDesc TextureDesc;

	FDescriptorHandle RTVDescriptorHandle = {};
};
