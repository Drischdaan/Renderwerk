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

	void ResourceBarrier(const TRef<FGfxTexture>& Texture, D3D12_RESOURCE_STATES NewResourceState) const;

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

private:
	D3D12_COMMAND_LIST_TYPE Type;
	TComPtr<ID3D12CommandAllocator> CommandAllocator;
	TComPtr<ID3D12GraphicsCommandList10> CommandList;
};
