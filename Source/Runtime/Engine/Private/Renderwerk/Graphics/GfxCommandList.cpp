#include "pch.hpp"

#include "Renderwerk/Graphics/GfxCommandList.hpp"

#include "Renderwerk/Graphics/GfxBuffer.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"
#include "Renderwerk/Graphics/Pipeline/GfxPipeline.hpp"

FGfxCommandList::FGfxCommandList(FGfxDevice* InGfxDevice, const D3D12_COMMAND_LIST_TYPE InType)
	: FGfxCommandList(InGfxDevice, InType, TEXT("UnnamedCommandList"))
{
}

FGfxCommandList::FGfxCommandList(FGfxDevice* InGfxDevice, const D3D12_COMMAND_LIST_TYPE InType, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName), Type(InType)
{
	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	HRESULT Result = NativeDevice->CreateCommandAllocator(Type, IID_PPV_ARGS(&CommandAllocator));
	RW_VERIFY_ID(Result);

	Result = NativeDevice->CreateCommandList(0, Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList));
	RW_VERIFY_ID(Result);

	Result = CommandList->Close();
	RW_VERIFY_ID(Result);
}

FGfxCommandList::~FGfxCommandList()
{
	CommandList.Reset();
	CommandAllocator.Reset();
}

void FGfxCommandList::Open() const
{
	HRESULT Result = CommandAllocator->Reset();
	RW_VERIFY_ID(Result);

	Result = CommandList->Reset(CommandAllocator.Get(), nullptr);
	RW_VERIFY_ID(Result);
}

void FGfxCommandList::Close() const
{
	const HRESULT Result = CommandList->Close();
	RW_VERIFY_ID(Result);
}

void FGfxCommandList::SetRenderTarget(const TRef<FGfxTexture>& Texture) const
{
	const D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = Texture->GetRTVDescriptorHandle().GetCPUHandle();
	CommandList->OMSetRenderTargets(1, &CPUHandle, true, nullptr);
}

void FGfxCommandList::ClearRenderTarget(const TRef<FGfxTexture>& Texture, const float32 Color[4]) const
{
	CommandList->ClearRenderTargetView(Texture->GetRTVDescriptorHandle().GetCPUHandle(), Color, 0, nullptr);
}

void FGfxCommandList::ResourceBarrier(const TRef<IGfxResource>& Resource, const D3D12_RESOURCE_STATES NewResourceState) const
{
	ID3D12Resource2* NativeResource = Resource->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
	const CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(NativeResource, Resource->GetResourceState(), NewResourceState);
	CommandList->ResourceBarrier(1, &ResourceBarrier);
	Resource->SetResourceState(NewResourceState);
}

void FGfxCommandList::ResourceBarrier(const TRef<FGfxTexture>& Texture, const D3D12_RESOURCE_STATES NewResourceState) const
{
	ID3D12Resource2* NativeResource = Texture->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
	const CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(NativeResource, Texture->GetResourceState(), NewResourceState);
	CommandList->ResourceBarrier(1, &ResourceBarrier);
	Texture->SetResourceState(NewResourceState);
}

void FGfxCommandList::CopyBufferToTexture(const TRef<IGfxResource>& Destination, const TRef<FGfxBuffer>& Source) const
{
	const D3D12_RESOURCE_DESC1 ResourceDesc = Destination->ResourceDesc;
	TVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> Footprints(ResourceDesc.MipLevels);
	TVector<uint32> NumRows(ResourceDesc.MipLevels);
	TVector<uint64> RowSizes(ResourceDesc.MipLevels);
	uint64 TotalSize = 0;

	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	NativeDevice->GetCopyableFootprints1(&ResourceDesc, 0, ResourceDesc.MipLevels, 0, Footprints.data(), NumRows.data(), RowSizes.data(), &TotalSize);

	ID3D12Resource2* NativeSourceResource = Source->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
	ID3D12Resource2* NativeDestinationResource = Destination->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
	for (uint32 MipLevel = 0; MipLevel < ResourceDesc.MipLevels; MipLevel++)
	{
		D3D12_TEXTURE_COPY_LOCATION SourceCopy = {};
		SourceCopy.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		SourceCopy.PlacedFootprint = Footprints[MipLevel];
		SourceCopy.pResource = NativeSourceResource;

		D3D12_TEXTURE_COPY_LOCATION DestinationCopy = {};
		DestinationCopy.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		DestinationCopy.SubresourceIndex = MipLevel;
		DestinationCopy.pResource = NativeDestinationResource;

		CommandList->CopyTextureRegion(&DestinationCopy, 0, 0, 0, &SourceCopy, nullptr);
	}
}

void FGfxCommandList::CopyTextureToTexture(const TRef<FGfxTexture>& Destination, const TRef<FGfxTexture>& Source) const
{
	ID3D12Resource2* NativeDestinationResource = Destination->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
	ID3D12Resource2* NativeSourceResource = Source->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
	CommandList->CopyResource(NativeDestinationResource, NativeSourceResource);
}

void FGfxCommandList::SetPipeline(const TRef<IGfxPipeline>& Pipeline) const
{
	ID3D12PipelineState* NativePipelineState = Pipeline->GetNativeObject<ID3D12PipelineState>(NativeObjectIds::D3D12_PipelineState);
	ID3D12RootSignature* NativeRootSignature = Pipeline->GetNativeObject<ID3D12RootSignature>(NativeObjectIds::D3D12_RootSignature);

	CommandList->SetPipelineState(NativePipelineState);
	CommandList->SetGraphicsRootSignature(NativeRootSignature);
}

void FGfxCommandList::SetTopology(const D3D12_PRIMITIVE_TOPOLOGY Topology) const
{
	CommandList->IASetPrimitiveTopology(Topology);
}

void FGfxCommandList::SetViewport(const uint32 Width, const uint32 Height) const
{
	D3D12_VIEWPORT Viewport;
	Viewport.Width = static_cast<float32>(Width);
	Viewport.Height = static_cast<float32>(Height);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;

	CommandList->RSSetViewports(1, &Viewport);
}

void FGfxCommandList::SetScissor(const uint32 Width, const uint32 Height, const uint32 Top, const uint32 Left) const
{
	D3D12_RECT Rect;
	Rect.right = static_cast<LONG>(Width);
	Rect.bottom = static_cast<LONG>(Height);
	Rect.top = static_cast<LONG>(Top);
	Rect.left = static_cast<LONG>(Left);

	CommandList->RSSetScissorRects(1, &Rect);
}

void FGfxCommandList::DrawInstanced(const uint32 VertexCount, const uint32 InstanceCount, const uint32 StartVertexLocation, const uint32 StartInstanceLocation) const
{
	CommandList->DrawInstanced(VertexCount, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

FNativeObject FGfxCommandList::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::D3D12_CommandList)
	{
		return CommandList.Get();
	}
	if (NativeObjectId == NativeObjectIds::D3D12_CommandAllocator)
	{
		return CommandAllocator.Get();
	}
	return IGfxDeviceChild::GetRawNativeObject(NativeObjectId);
}
