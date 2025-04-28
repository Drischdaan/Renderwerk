#include "pch.hpp"

#include "Renderwerk/Graphics/GfxResourceManager.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Graphics/GfxBuffer.hpp"
#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"
#include "Renderwerk/Job/JobModule.hpp"
#include "Renderwerk/Platform/Threading/ScopedLock.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FGfxUploadJob::FGfxUploadJob(FGfxDevice* InGfxDevice, const TVector<FGfxUploadRequest>& InUploadRequests)
	: GfxDevice(InGfxDevice), UploadRequests(InUploadRequests)
{
}

void FGfxUploadJob::Execute()
{
	const TRef<FGfxCommandList> CommandList = GfxDevice->CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
	CommandList->Open();
	{
		PROFILE_SCOPE("UploadResources");
		if (UploadRequests.empty())
		{
			return;
		}
		ID3D12GraphicsCommandList10* NativeCommandList = CommandList->GetNativeObject<ID3D12GraphicsCommandList10>(NativeObjectIds::D3D12_CommandList);
		for (const FGfxUploadRequest& UploadRequest : UploadRequests)
		{
			RW_LOG(Debug, "Processing upload request for '{}'", UploadRequest.Resource->GetDebugName());
			switch (UploadRequest.Type)
			{
			case EGfxUploadRequestType::Buffer:
				{
					ID3D12Resource2* NativeUploadResource = UploadRequest.Resource->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
					ID3D12Resource2* NativeStagingResource = UploadRequest.StagingBuffer->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
					CommandList->ResourceBarrier(UploadRequest.Resource, D3D12_RESOURCE_STATE_COPY_DEST);
					CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE);
					NativeCommandList->CopyResource(NativeUploadResource, NativeStagingResource);
					CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COMMON);
				}
				break;
			case EGfxUploadRequestType::Texture:
				{
					CommandList->ResourceBarrier(UploadRequest.Resource, D3D12_RESOURCE_STATE_COPY_DEST);
					CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE);
					CommandList->CopyBufferToTexture(UploadRequest.Resource, UploadRequest.StagingBuffer);
					CommandList->ResourceBarrier(UploadRequest.StagingBuffer, D3D12_RESOURCE_STATE_COMMON);
				}
				break;
			}
		}
	}
	CommandList->Close();
	GfxDevice->SubmitCopyWork(CommandList);
	GfxDevice->FlushCopyQueue();
	UploadRequests.clear();
}

FGfxResourceManager::FGfxResourceManager(FGfxDevice* InGfxDevice, const FGfxResourceManagerDesc& InResourceManagerDesc)
	: FGfxResourceManager(InGfxDevice, InResourceManagerDesc, TEXT("UnnamedResourceManager"))
{
}

FGfxResourceManager::FGfxResourceManager(FGfxDevice* InGfxDevice, const FGfxResourceManagerDesc& InResourceManagerDesc, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName), ResourceManagerDesc(InResourceManagerDesc), TexturePool(InResourceManagerDesc.MaxTextures),
	  BufferPool(InResourceManagerDesc.MaxBuffers)
{
}

FGfxResourceManager::~FGfxResourceManager()
{
	UploadRequests.clear();
	Buffers.clear();
	BufferPool.Destroy();
	Textures.clear();
	TexturePool.Destroy();
}

TRef<FGfxTexture> FGfxResourceManager::AllocateTexture(const FGfxTextureDesc& TextureDesc, const FStringView& DebugName)
{
	RW_VERIFY_MSG(TexturePool.GetUsage() + 1 <= ResourceManagerDesc.MaxTextures, "No space to allocate new texture '{}'", DebugName);
	FGfxTexture* TexturePointer = TexturePool.Allocate();
	TRef<FGfxTexture> Texture = NewRefWithDeleter<FGfxTexture>(TexturePointer, FGfxTextureDeleter(this), GfxDevice, TextureDesc, DebugName);
	Textures.push_back(Texture);
	return Texture;
}

void FGfxResourceManager::DestroyTexture(TRef<FGfxTexture>&& Texture)
{
	RW_VERIFY_MSG(TexturePool.Contains(Texture.get()), "Given texture ('{}') was not created through the resource manager", Texture->GetDebugName());
	const uint64 RefCount = Texture.use_count();
	RW_VERIFY_MSG(RefCount == 2, "Trying to delete texture ('{}') that is still referenced", Texture->GetDebugName());
	std::move(Texture).reset();
	const auto Iterator = std::ranges::find(Textures.begin(), Textures.end(), Texture);
	if (Iterator != Textures.end())
	{
		Textures.erase(Iterator);
	}
}

TRef<FGfxBuffer> FGfxResourceManager::AllocateBuffer(EGfxBufferType Type, uint64 Size, uint32 Stride, const FStringView& DebugName)
{
	RW_VERIFY_MSG(BufferPool.GetUsage() + 1 <= ResourceManagerDesc.MaxBuffers, "No space to allocate new texture '{}'", DebugName);
	FGfxBuffer* BufferPointer = BufferPool.Allocate();
	TRef<FGfxBuffer> Buffer = NewRefWithDeleter<FGfxBuffer>(BufferPointer, FGfxBufferDeleter(this), GfxDevice, Type, Size, Stride, DebugName);
	Buffers.push_back(Buffer);
	return Buffer;
}

void FGfxResourceManager::DestroyBuffer(TRef<FGfxBuffer>&& Buffer)
{
	RW_VERIFY_MSG(BufferPool.Contains(Buffer.get()), "Given buffer ('{}') was not created through the resource manager", Buffer->GetDebugName());
	const uint64 RefCount = Buffer.use_count();
	RW_VERIFY_MSG(RefCount == 2, "Trying to delete buffer ('{}') that is still referenced", Buffer->GetDebugName());
	std::move(Buffer).reset();
	const auto Iterator = std::ranges::find(Buffers.begin(), Buffers.end(), Buffer);
	if (Iterator != Buffers.end())
	{
		Buffers.erase(Iterator);
	}
}

void FGfxResourceManager::CollectResourceUploads()
{
	PROFILE_FUNCTION();
	for (TRef<FGfxTexture>& Texture : Textures)
	{
		if (Texture->IsDirty())
		{
			QueueTextureUpload(Texture);
		}
	}
	for (TRef<FGfxBuffer>& Buffer : Buffers)
	{
		if (Buffer->IsDirty())
		{
			QueueBufferUpload(Buffer);
		}
	}

	if (!UploadRequests.empty())
	{
		const TRef<FJobModule> JobModule = GetEngine()->GetModule<FJobModule>();
		JobModule->QueueJob(NewRef<FGfxUploadJob>(GfxDevice, UploadRequests));
		UploadRequests.clear();
	}
}

void FGfxResourceManager::QueueTextureUpload(const TRef<FGfxTexture>& Texture)
{
	FGfxUploadRequest Request = {};
	Request.Type = EGfxUploadRequestType::Texture;
	Request.Resource = Texture;

	TVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> Footprints(Texture->GetMipLevels());
	TVector<uint32> NumRows(Texture->GetMipLevels());
	TVector<uint64> RowSizes(Texture->GetMipLevels());
	uint64 TotalSize = 0;

	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	NativeDevice->GetCopyableFootprints1(&Texture->ResourceDesc, 0, Texture->GetMipLevels(), 0, Footprints.data(), NumRows.data(), RowSizes.data(), &TotalSize);
	Request.StagingBuffer = NewRef<FGfxBuffer>(GfxDevice, EGfxBufferType::Copy, TotalSize, 0);

	const uint8* Pixels = Texture->GetData().data();
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
	Texture->ResetDirtyState();
}

void FGfxResourceManager::QueueBufferUpload(const TRef<FGfxBuffer>& Buffer)
{
	FGfxUploadRequest Request = {};
	Request.Type = EGfxUploadRequestType::Buffer;
	Request.Resource = Buffer;
	Request.StagingBuffer = NewRef<FGfxBuffer>(GfxDevice, EGfxBufferType::Copy, Buffer->GetDataSize(), Buffer->GetStride(),
	                                           Buffer->GetDebugName() + TEXT("Staging"));

	void* MappedPointer = nullptr;
	Request.StagingBuffer->Map(&MappedPointer);
	FMemory::Copy(MappedPointer, Buffer->GetData(), Buffer->GetDataSize());
	Request.StagingBuffer->Unmap();

	FScopedLock Lock(&RequestSection);
	UploadRequests.push_back(Request);
	Buffer->ResetDirtyState();
}
