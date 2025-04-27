#pragma once

#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

class ENGINE_API FGfxCommandList : public IGfxDeviceChild
{
public:
	explicit FGfxCommandList(FGfxDevice* InGfxDevice, D3D12_COMMAND_LIST_TYPE InType);
	FGfxCommandList(FGfxDevice* InGfxDevice, D3D12_COMMAND_LIST_TYPE InType, const FStringView& InDebugName);
	~FGfxCommandList() override;

	NON_COPY_MOVEABLE(FGfxCommandList)

public:
	void Open() const;
	void Close() const;

	void SetRenderTarget(const TRef<FGfxTexture>& Texture) const;
	void ClearRenderTarget(const TRef<FGfxTexture>& Texture, const float32 Color[4]) const;

	void ResourceBarrier(const TRef<IGfxResource>& Resource, D3D12_RESOURCE_STATES NewResourceState) const;
	void ResourceBarrier(const TRef<FGfxTexture>& Texture, D3D12_RESOURCE_STATES NewResourceState) const;

	void CopyBufferToTexture(const TRef<IGfxResource>& Destination, const TRef<FGfxBuffer>& Source) const;

	void SetPipeline(const TRef<IGfxPipeline>& Pipeline) const;
	void SetTopology(D3D12_PRIMITIVE_TOPOLOGY Topology) const;
	void SetViewport(uint32 Width, uint32 Height) const;
	void SetScissor(uint32 Width, uint32 Height, uint32 Top = 0, uint32 Left = 0) const;

	void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation = 0, uint32 StartInstanceLocation = 0) const;

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

private:
	D3D12_COMMAND_LIST_TYPE Type;
	TComPtr<ID3D12CommandAllocator> CommandAllocator;
	TComPtr<ID3D12GraphicsCommandList10> CommandList;
};
