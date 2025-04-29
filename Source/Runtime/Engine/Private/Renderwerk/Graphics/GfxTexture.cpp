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
	if (TextureDesc.Usage & EGfxTextureUsage::DepthTarget)
	{
		AllocateDepthStencil();
	}
	if (TextureDesc.Usage & EGfxTextureUsage::SharedResource)
	{
		AllocateSharedSource();
	}
}

FGfxTexture::~FGfxTexture()
{
	if (TextureDesc.Usage & EGfxTextureUsage::RenderTarget)
	{
		GfxDevice->GetRTVDescriptorHeap()->Free(RTVDescriptorHandle);
	}
	if (TextureDesc.Usage & EGfxTextureUsage::DepthTarget)
	{
		GfxDevice->GetDSVDescriptorHeap()->Free(DSVDescriptorHandle);
	}
	if (TextureDesc.Usage & EGfxTextureUsage::SharedResource)
	{
		GfxDevice->GetSRVDescriptorHeap()->Free(SRVDescriptorHandle);
	}
}

void FGfxTexture::SetData(const TVector<uint8>& NewData)
{
	Data = NewData;
	bIsDirty = true;
}

uint32 FGfxTexture::GetWidth() const
{
	return TextureDesc.Width;
}

uint32 FGfxTexture::GetHeight() const
{
	return TextureDesc.Height;
}

uint32 FGfxTexture::GetMipLevels() const
{
	return TextureDesc.MipLevels;
}

EGfxTextureUsage FGfxTexture::GetUsage() const
{
	return TextureDesc.Usage;
}

DXGI_FORMAT FGfxTexture::GetFormat() const
{
	return TextureDesc.Format;
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

void FGfxTexture::AllocateDepthStencil()
{
	DSVDescriptorHandle = GfxDevice->GetDSVDescriptorHeap()->Allocate();

	D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
	DepthStencilViewDesc.Format = TextureDesc.Format;

	switch (TextureDesc.Dimension)
	{
	case EGfxTextureDimension::Texture1D:
		DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
		DepthStencilViewDesc.Texture1D.MipSlice = 0;
		break;
	case EGfxTextureDimension::Texture2D:
		DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DepthStencilViewDesc.Texture2D.MipSlice = 0;
		break;
	case EGfxTextureDimension::Texture3D:
		break;
	}

	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	NativeDevice->CreateDepthStencilView(Resource.Get(), &DepthStencilViewDesc, DSVDescriptorHandle.GetCPUHandle());
}

void FGfxTexture::AllocateSharedSource()
{
	SRVDescriptorHandle = GfxDevice->GetSRVDescriptorHeap()->Allocate();

	D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc = {};
	ShaderResourceViewDesc.Format = TextureDesc.Format;
	ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (TextureDesc.Dimension)
	{
	case EGfxTextureDimension::Texture1D:
		ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
		ShaderResourceViewDesc.Texture1D.MipLevels = TextureDesc.MipLevels;
		ShaderResourceViewDesc.Texture1D.MostDetailedMip = 0;
		break;
	case EGfxTextureDimension::Texture2D:
		ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceViewDesc.Texture2D.MipLevels = TextureDesc.MipLevels;
		ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		break;
	case EGfxTextureDimension::Texture3D:
		ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		ShaderResourceViewDesc.Texture3D.MipLevels = TextureDesc.MipLevels;
		ShaderResourceViewDesc.Texture3D.MostDetailedMip = 0;
		break;
	}

	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	NativeDevice->CreateShaderResourceView(Resource.Get(), &ShaderResourceViewDesc, SRVDescriptorHandle.GetCPUHandle());
}
