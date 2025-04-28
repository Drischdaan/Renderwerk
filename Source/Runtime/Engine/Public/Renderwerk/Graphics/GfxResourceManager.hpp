#pragma once

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/PoolAllocator.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxBuffer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"
#include "Renderwerk/Job/Job.hpp"

enum class ENGINE_API EGfxUploadRequestType : uint8
{
	Buffer = 0,
	Texture,
};

struct ENGINE_API FGfxUploadRequest
{
	EGfxUploadRequestType Type;
	TRef<IGfxResource> Resource;
	TRef<FGfxBuffer> StagingBuffer;
};

struct ENGINE_API FGfxResourceManagerDesc
{
	uint64 MaxTextures = 1024;
	uint64 MaxBuffers = 1024;
};

class ENGINE_API FGfxUploadJob : public IJob
{
public:
	FGfxUploadJob(FGfxDevice* InGfxDevice, const TVector<FGfxUploadRequest>& InUploadRequests);
	~FGfxUploadJob() override = default;

	NON_COPY_MOVEABLE(FGfxUploadJob)

public:
	void Execute() override;

private:
	FGfxDevice* GfxDevice;
	TVector<FGfxUploadRequest> UploadRequests;
};

class ENGINE_API FGfxResourceManager : public IGfxDeviceChild
{
private:
	struct FGfxTextureDeleter
	{
		FGfxResourceManager* ResourceManager;

		explicit FGfxTextureDeleter(FGfxResourceManager* InResourceManager)
			: ResourceManager(InResourceManager)
		{
		}

		void operator()(FGfxTexture* Texture) const
		{
			ResourceManager->TexturePool.Free(Texture);
		}
	};

	struct FGfxBufferDeleter
	{
		FGfxResourceManager* ResourceManager;

		explicit FGfxBufferDeleter(FGfxResourceManager* InResourceManager)
			: ResourceManager(InResourceManager)
		{
		}

		void operator()(FGfxBuffer* Buffer) const
		{
			ResourceManager->BufferPool.Free(Buffer);
		}
	};

public:
	explicit FGfxResourceManager(FGfxDevice* InGfxDevice, const FGfxResourceManagerDesc& InResourceManagerDesc);
	FGfxResourceManager(FGfxDevice* InGfxDevice, const FGfxResourceManagerDesc& InResourceManagerDesc, const FStringView& InDebugName);
	~FGfxResourceManager() override;

	NON_COPY_MOVEABLE(FGfxResourceManager)

public:
	[[nodiscard]] TRef<FGfxTexture> AllocateTexture(const FGfxTextureDesc& TextureDesc, const FStringView& DebugName = TEXT("UnnamedTexture"));
	void DestroyTexture(TRef<FGfxTexture>&& Texture);

	[[nodiscard]] TRef<FGfxBuffer> AllocateBuffer(EGfxBufferType Type, uint64 Size, uint32 Stride, const FStringView& DebugName = TEXT("UnnamedBuffer"));
	void DestroyBuffer(TRef<FGfxBuffer>&& Buffer);

	void CollectResourceUploads();

	void QueueTextureUpload(const TRef<FGfxTexture>& Texture);
	void QueueBufferUpload(const TRef<FGfxBuffer>& Buffer);

private:
	FGfxResourceManagerDesc ResourceManagerDesc;

	TPoolAllocator<FGfxTexture> TexturePool;
	TVector<TRef<FGfxTexture>> Textures;

	TPoolAllocator<FGfxBuffer> BufferPool;
	TVector<TRef<FGfxBuffer>> Buffers;

	FCriticalSection RequestSection;
	TVector<FGfxUploadRequest> UploadRequests;
};
