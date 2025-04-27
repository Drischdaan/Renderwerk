#pragma once

#include "Renderwerk/Graphics/GfxCommon.hpp"

class ENGINE_API IGfxResource : public IGfxDeviceChild
{
public:
	explicit IGfxResource(FGfxDevice* InGfxDevice);
	IGfxResource(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	virtual ~IGfxResource() override;

	NON_COPY_MOVEABLE(IGfxResource)

public:
	void SetResourceState(D3D12_RESOURCE_STATES NewResourceState);

	void ResetDirtyState();

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

public:
	[[nodiscard]] D3D12_RESOURCE_STATES GetResourceState() const { return ResourceState; }
	[[nodiscard]] uint64 GetAllocationSize() const { return AllocationSize; }

	[[nodiscard]] bool8 IsDirty() const { return bIsDirty; }

protected:
	void CreateResource(const D3D12_HEAP_PROPERTIES* HeapProperties);

protected:
	CD3DX12_RESOURCE_DESC1 ResourceDesc = {};
	TComPtr<ID3D12Resource2> Resource;

	D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATE_COMMON;
	uint64 AllocationSize = 0;

	bool8 bIsDirty = false; // Indicates if the resource changed and needs uploading

	friend class ENGINE_API FGfxResourceManager;
	friend class ENGINE_API FGfxCommandList;
};
