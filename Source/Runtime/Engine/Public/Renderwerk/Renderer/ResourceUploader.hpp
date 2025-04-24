#pragma once

#include "CommandList.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/Queue.hpp"
#include "Renderwerk/Renderer/Buffer.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/Resource.hpp"
#include "Renderwerk/Renderer/Texture.hpp"

enum class ENGINE_API EUploadRequestType : uint8
{
	Buffer = 0,
	Texture,
};

struct ENGINE_API FUploadRequest
{
	EUploadRequestType Type;
	TObjectHandle<IResource> Resource;
	TObjectHandle<FBuffer> StagingBuffer;
};

class ENGINE_API FResourceUploader : public TGraphicsObject<FResourceUploader>
{
public:
	FResourceUploader(FDevice* InDevice);
	~FResourceUploader() override;

	NON_COPY_MOVEABLE(FResourceUploader)

public:
	void QueueTextureUpload(TObjectHandle<FTexture> Texture, const TVector<uint8>& Data);
	void QueueBufferUpload(TObjectHandle<FBuffer> Buffer, const void* Data, size64 DataSize);

	void Flush(const TObjectHandle<FCommandList>& CommandList);
	void ReleaseResources();

private:
	FDevice* Device;

	FCriticalSection RequestSection;
	TVector<FUploadRequest> UploadRequests;
};
