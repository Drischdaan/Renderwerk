#include "pch.hpp"

#include "Renderwerk/Renderer/ResourceUploader.hpp"

#include "Renderwerk/Platform/Threading/ScopedLock.hpp"
#include "Renderwerk/Renderer/Device.hpp"

FResourceUploader::FResourceUploader(FDevice* InDevice)
	: Device(InDevice)
{
}

FResourceUploader::~FResourceUploader()
{
}

void FResourceUploader::QueueTextureUpload(TObjectHandle<FTexture> Texture, const TVector<uint8>& Data) // NOLINT(performance-unnecessary-value-param)
{
	FUploadRequest Request = {};
	Request.Type = EUploadRequestType::Texture;
	Request.Resource = Texture;

	TVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> Footprints(Texture->GetMipLevels());
	TVector<uint32> NumRows(Texture->GetMipLevels());
	TVector<uint64> RowSizes(Texture->GetMipLevels());
	uint64 TotalSize = 0;

	Device->GetHandle()->GetCopyableFootprints1(&Texture->ResourceDesc, 0, Texture->GetMipLevels(), 0, Footprints.data(), NumRows.data(), RowSizes.data(), &TotalSize);
	Request.StagingBuffer = NewObjectHandle<FBuffer>(Device, EBufferType::Copy, TotalSize, 0);

	const uint8* Pixels = Data.data();
	uint8* MappedPointer = nullptr;
	Request.StagingBuffer->Map(reinterpret_cast<void**>(&MappedPointer));
	for (uint32 MipLevel = 0; MipLevel < Texture->GetMipLevels(); ++MipLevel)
	{
		for (uint32 Row = 0; Row < NumRows.at(MipLevel); ++Row)
		{
			FMemory::Copy(MappedPointer, Pixels, RowSizes.at(MipLevel));
			MappedPointer += Footprints.at(MipLevel).Footprint.RowPitch;
			Pixels += RowSizes.at(MipLevel);
		}
	}
	Request.StagingBuffer->Unmap();

	FScopedLock Lock(&RequestSection);
	UploadRequests.push_back(Request);
}

void FResourceUploader::QueueBufferUpload(TObjectHandle<FBuffer> Buffer, const void* Data, size64 DataSize) // NOLINT(performance-unnecessary-value-param)
{
	FUploadRequest Request = {};
	Request.Type = EUploadRequestType::Buffer;
	Request.Resource = Buffer;
	Request.StagingBuffer = NewObjectHandle<FBuffer>(Device, EBufferType::Copy, DataSize, 0);

	void* MappedPointer = nullptr;
	Request.StagingBuffer->Map(&MappedPointer);
	FMemory::Copy(MappedPointer, Data, DataSize);
	Request.StagingBuffer->Unmap();

	FScopedLock Lock(&RequestSection);
	UploadRequests.push_back(Request);
}

void FResourceUploader::Flush(const TObjectHandle<FCommandList>& CommandList)
{
	FScopedLock Lock(&RequestSection);
	if (UploadRequests.empty())
	{
		return;
	}
	for (const FUploadRequest& UploadRequest : UploadRequests)
	{
		switch (UploadRequest.Type)
		{
		case EUploadRequestType::Buffer:
			CommandList->ResourceBarrier(UploadRequest.Resource, D3D12_RESOURCE_STATE_COPY_DEST);
			CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE);
			CommandList->GetHandle()->CopyResource(UploadRequest.Resource->GetResource(), UploadRequest.StagingBuffer->GetResource());
			CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COMMON);
			CommandList->ResourceBarrier(UploadRequest.Resource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			break;
		case EUploadRequestType::Texture:
			CommandList->ResourceBarrier(UploadRequest.Resource, D3D12_RESOURCE_STATE_COPY_DEST);
			CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE);
			CommandList->CopyBufferToTexture(UploadRequest.Resource, UploadRequest.StagingBuffer);
			CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COMMON);
			CommandList->ResourceBarrier(UploadRequest.Resource, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
			break;
		}
	}
}

void FResourceUploader::ReleaseResources()
{
	UploadRequests.clear();
}
