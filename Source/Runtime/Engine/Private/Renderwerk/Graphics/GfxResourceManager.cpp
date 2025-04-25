#include "pch.hpp"

#include "Renderwerk/Graphics/GfxResourceManager.hpp"

#include "Renderwerk/Graphics/GfxTexture.hpp"

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

TRef<FGfxBuffer> FGfxResourceManager::AllocateBuffer(const FStringView& DebugName)
{
	RW_VERIFY_MSG(BufferPool.GetUsage() + 1 <= ResourceManagerDesc.MaxBuffers, "No space to allocate new texture '{}'", DebugName);
	FGfxBuffer* BufferPointer = BufferPool.Allocate();
	TRef<FGfxBuffer> Buffer = NewRefWithDeleter<FGfxBuffer>(BufferPointer, FGfxBufferDeleter(this), GfxDevice, DebugName);
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
