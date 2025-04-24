#include "pch.hpp"

#include "Renderwerk/Renderer/CommandList.hpp"

#include "Renderwerk/Renderer/Device.hpp"

FCommandList::FCommandList(FDevice* InDevice, const D3D12_COMMAND_LIST_TYPE InType)
	: Device(InDevice), Type(InType)
{
	HRESULT Result = Device->GetHandle()->CreateCommandAllocator(Type, IID_PPV_ARGS(&CommandAllocator));
	RW_VERIFY_ID(Result);

	Result = Device->GetHandle()->CreateCommandList(0, Type, CommandAllocator, nullptr, IID_PPV_ARGS(&CommandList));
	RW_VERIFY_ID(Result);
	Close();
}

FCommandList::~FCommandList()
{
	CommandList.Reset();
	CommandAllocator.Reset();
}

void FCommandList::Open() const
{
	HRESULT Result = CommandAllocator->Reset();
	RW_VERIFY_ID(Result);

	Result = CommandList->Reset(CommandAllocator, nullptr);
	RW_VERIFY_ID(Result);
}

void FCommandList::Close() const
{
	const HRESULT Result = CommandList->Close();
	RW_VERIFY_ID(Result);
}

void FCommandList::ClearRenderTarget(const TObjectHandle<FTexture>& Texture, const float32 Colors[4]) const
{
	RW_ASSERT(Texture->GetRTVDescriptorHandle().IsValid());
	CommandList->ClearRenderTargetView(Texture->GetRTVDescriptorHandle().GetCPUHandle(), Colors, 0, nullptr);
}

void FCommandList::SetRenderTarget(const TObjectHandle<FTexture>& Texture) const
{
	RW_ASSERT(Texture->GetRTVDescriptorHandle().IsValid());
	const D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = Texture->GetRTVDescriptorHandle().GetCPUHandle();
	CommandList->OMSetRenderTargets(1, &CPUHandle, true, nullptr);
}

void FCommandList::ResourceBarrier(const TObjectHandle<IResource>& Resource, const D3D12_RESOURCE_STATES NewState) const
{
	const CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Resource->GetResource(), Resource->GetResourceState(), NewState);
	CommandList->ResourceBarrier(1, &ResourceBarrier);
	Resource->ChangeResourceState(NewState);
}

void FCommandList::ResourceBarrier(const TObjectHandle<FTexture>& Texture, const D3D12_RESOURCE_STATES NewState) const
{
	const CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Texture->GetResource(), Texture->GetResourceState(), NewState);
	CommandList->ResourceBarrier(1, &ResourceBarrier);
	Texture->ChangeResourceState(NewState);
}

void FCommandList::ResourceBarrier(const TObjectHandle<FBuffer>& Buffer, const D3D12_RESOURCE_STATES NewState) const
{
	const CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Buffer->GetResource(), Buffer->GetResourceState(), NewState);
	CommandList->ResourceBarrier(1, &ResourceBarrier);
	Buffer->ChangeResourceState(NewState);
}

void FCommandList::CopyBufferToTexture(const TObjectHandle<IResource>& Destination, const TObjectHandle<FBuffer>& Source) const
{
	const D3D12_RESOURCE_DESC1 ResourceDesc = Destination->ResourceDesc;
	TVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> Footprints(ResourceDesc.MipLevels);
	TVector<uint32> NumRows(ResourceDesc.MipLevels);
	TVector<uint64> RowSizes(ResourceDesc.MipLevels);
	uint64 TotalSize = 0;

	Device->GetHandle()->GetCopyableFootprints1(&ResourceDesc, 0, ResourceDesc.MipLevels, 0, Footprints.data(), NumRows.data(), RowSizes.data(), &TotalSize);

	for (uint32 MipLevel = 0; MipLevel < ResourceDesc.MipLevels; MipLevel++)
	{
		D3D12_TEXTURE_COPY_LOCATION SourceCopy = {};
		SourceCopy.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		SourceCopy.PlacedFootprint = Footprints[MipLevel];
		SourceCopy.pResource = Source->GetResource();

		D3D12_TEXTURE_COPY_LOCATION DestinationCopy = {};
		DestinationCopy.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		DestinationCopy.SubresourceIndex = MipLevel;
		DestinationCopy.pResource = Destination->GetResource();

		CommandList->CopyTextureRegion(&DestinationCopy, 0, 0, 0, &SourceCopy, nullptr);
	}
}
