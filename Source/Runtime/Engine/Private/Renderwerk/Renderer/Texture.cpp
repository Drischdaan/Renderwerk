#include "pch.hpp"

#include "Renderwerk/Renderer/Texture.hpp"

#include "Renderwerk/Renderer/Device.hpp"

FTexture::FTexture(FDevice* InDevice, const FTextureDesc& InTextureDesc, const TObjectHandle<ID3D12Resource2>& InResource)
	: IResource(InDevice), TextureDesc(InTextureDesc)
{
	Resource = InResource;
	ResourceState = TextureDesc.InitialState;

	ResourceDesc.Width = TextureDesc.Width;
	ResourceDesc.Height = TextureDesc.Height;
	ResourceDesc.Format = TextureDesc.Format;
	ResourceDesc.MipLevels = TextureDesc.MipLevels;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	switch (TextureDesc.Dimension)
	{
	case ETextureDimension::Texture1D:
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		break;
	case ETextureDimension::Texture2D:
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		break;
	case ETextureDimension::Texture3D:
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		break;
	}

	if (TextureDesc.Usage & ETextureUsage::RenderTarget)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}
	if (TextureDesc.Usage & ETextureUsage::Storage)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	if (TextureDesc.Usage & ETextureUsage::DepthTarget)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	}

	if (!Resource)
	{
		CD3DX12_HEAP_PROPERTIES HeapProperties = {};
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		CreateResource(&HeapProperties);
	}

	if (TextureDesc.Usage & ETextureUsage::RenderTarget)
	{
		AllocateRenderTarget();
	}

	if (!TextureDesc.DebugName.empty())
	{
		Resource->SetName(TextureDesc.DebugName.data());
	}
}

FTexture::FTexture(FDevice* InDevice, const FTextureDesc& InTextureDesc)
	: FTexture(InDevice, InTextureDesc, nullptr)
{
}

FTexture::~FTexture()
{
	if (RTVDescriptorHandle.IsValid())
	{
		Device->GetRTVDescriptorHeap()->Free(RTVDescriptorHandle);
	}
}

uint32 FTexture::GetWidth() const
{
	return TextureDesc.Width;
}

uint32 FTexture::GetHeight() const
{
	return TextureDesc.Height;
}

uint32 FTexture::GetMipLevels() const
{
	return TextureDesc.MipLevels;
}

ETextureUsage FTexture::GetUsage() const
{
	return TextureDesc.Usage;
}

DXGI_FORMAT FTexture::GetFormat() const
{
	return TextureDesc.Format;
}

void FTexture::AllocateRenderTarget()
{
	RTVDescriptorHandle = Device->GetRTVDescriptorHeap()->Allocate();

	D3D12_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc = {};
	RenderTargetViewDesc.Format = TextureDesc.Format;

	switch (TextureDesc.Dimension)
	{
	case ETextureDimension::Texture1D:
		RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
		RenderTargetViewDesc.Texture1D.MipSlice = 0;
		break;
	case ETextureDimension::Texture2D:
		RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RenderTargetViewDesc.Texture2D.MipSlice = 0;
		RenderTargetViewDesc.Texture2D.PlaneSlice = 0;
		break;
	case ETextureDimension::Texture3D:
		RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
		RenderTargetViewDesc.Texture3D.MipSlice = 0;
		break;
	}

	Device->GetHandle()->CreateRenderTargetView(Resource, &RenderTargetViewDesc, RTVDescriptorHandle.GetCPUHandle());
}
