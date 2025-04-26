#include "pch.hpp"

#include "Renderwerk/Graphics/GfxTexture.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"

FGfxTexture::FGfxTexture(FGfxDevice* InGfxDevice, const FGfxTextureDesc& InTextureDesc, const TComPtr<ID3D12Resource2>& InResource)
	: FGfxTexture(InGfxDevice, InTextureDesc, InResource, TEXT("UnnamedTexture"))
{
}

FGfxTexture::FGfxTexture(FGfxDevice* InGfxDevice, const FGfxTextureDesc& InTextureDesc, const FStringView& InDebugName)
	: FGfxTexture(InGfxDevice, InTextureDesc, nullptr, InDebugName)
{
}

FGfxTexture::FGfxTexture(FGfxDevice* InGfxDevice, const FGfxTextureDesc& InTextureDesc, const TComPtr<ID3D12Resource2>& InResource, const FStringView& InDebugName)
	: IGfxResource(InGfxDevice, InDebugName), TextureDesc(InTextureDesc)
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
	case EGfxTextureDimension::Texture1D:
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		break;
	case EGfxTextureDimension::Texture2D:
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		break;
	case EGfxTextureDimension::Texture3D:
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		break;
	}

	if (TextureDesc.Usage & EGfxTextureUsage::RenderTarget)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}
	if (TextureDesc.Usage & EGfxTextureUsage::Storage)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	if (TextureDesc.Usage & EGfxTextureUsage::DepthTarget)
	{
		ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	}

	if (!Resource)
	{
		CD3DX12_HEAP_PROPERTIES HeapProperties = {};
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		CreateResource(&HeapProperties);
	}

	if (TextureDesc.Usage & EGfxTextureUsage::RenderTarget)
	{
		AllocateRenderTarget();
	}
}

FGfxTexture::~FGfxTexture()
{
	if (TextureDesc.Usage & EGfxTextureUsage::RenderTarget)
	{
		GfxDevice->GetRTVDescriptorHeap()->Free(RTVDescriptorHandle);
	}
}

void FGfxTexture::AllocateRenderTarget()
{
	RTVDescriptorHandle = GfxDevice->GetRTVDescriptorHeap()->Allocate();

	D3D12_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc = {};
	RenderTargetViewDesc.Format = TextureDesc.Format;

	switch (TextureDesc.Dimension)
	{
	case EGfxTextureDimension::Texture1D:
		RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
		RenderTargetViewDesc.Texture1D.MipSlice = 0;
		break;
	case EGfxTextureDimension::Texture2D:
		RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RenderTargetViewDesc.Texture2D.MipSlice = 0;
		RenderTargetViewDesc.Texture2D.PlaneSlice = 0;
		break;
	case EGfxTextureDimension::Texture3D:
		RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
		RenderTargetViewDesc.Texture3D.MipSlice = 0;
		break;
	}

	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	NativeDevice->CreateRenderTargetView(Resource.Get(), &RenderTargetViewDesc, RTVDescriptorHandle.GetCPUHandle());
}
