#pragma once

#include "Renderwerk/Graphics/GfxResource.hpp"

#include "Renderwerk/Graphics/GfxDescriptorHeap.hpp"

enum class ENGINE_API EGfxTextureUsage : uint8
{
	RenderTarget = BIT(1),
	DepthTarget = BIT(2),
	Storage = BIT(3),
	SharedResource = BIT(4),
};

DEFINE_ENUM_FLAGS(EGfxTextureUsage)

enum class ENGINE_API EGfxTextureDimension : uint8
{
	Texture1D = 0,
	Texture2D,
	Texture3D,
};


struct ENGINE_API FGfxTextureDesc
{
	EGfxTextureUsage Usage;
	uint32 Width = 0;
	uint32 Height = 0;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uint16 MipLevels = 0;
	EGfxTextureDimension Dimension = EGfxTextureDimension::Texture2D;
	D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_COMMON;
	FStringView DebugName;
};

class ENGINE_API FGfxTexture : public IGfxResource
{
public:
	explicit FGfxTexture(FGfxDevice* InGfxDevice, const FGfxTextureDesc& InTextureDesc, const TComPtr<ID3D12Resource2>& InResource = nullptr);
	FGfxTexture(FGfxDevice* InGfxDevice, const FGfxTextureDesc& InTextureDesc, const FStringView& InDebugName);
	FGfxTexture(FGfxDevice* InGfxDevice, const FGfxTextureDesc& InTextureDesc, const TComPtr<ID3D12Resource2>& InResource, const FStringView& InDebugName);
	~FGfxTexture() override;

	NON_COPY_MOVEABLE(FGfxTexture)

public:
	[[nodiscard]] FGfxDescriptorHandle GetRTVDescriptorHandle() const { return RTVDescriptorHandle; }

private:
	void AllocateRenderTarget();

private:
	FGfxTextureDesc TextureDesc;

	FGfxDescriptorHandle RTVDescriptorHandle;
};
