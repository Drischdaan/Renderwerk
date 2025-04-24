#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FDevice;

class ENGINE_API IResource : public TGraphicsObject<IResource>
{
public:
	IResource(FDevice* InDevice);
	~IResource() override;

	NON_COPY_MOVEABLE(IResource)

public:
	void ChangeResourceState(D3D12_RESOURCE_STATES NewState);

	[[nodiscard]] uint64 GetVirtualAddress() const;

public:
	[[nodiscard]] TObjectHandle<ID3D12Resource2> GetResource() const { return Resource; }

	[[nodiscard]] D3D12_RESOURCE_STATES GetResourceState() const { return ResourceState; }
	[[nodiscard]] uint64 GetAllocationSize() const { return AllocationSize; }

protected:
	void CreateResource(const D3D12_HEAP_PROPERTIES* HeapProperties);

protected:
	FDevice* Device;

	D3D12_RESOURCE_DESC1 ResourceDesc = {};
	TObjectHandle<ID3D12Resource2> Resource;
	D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATE_COMMON;

	uint64 AllocationSize = 0;

	friend class ENGINE_API FCommandList;
	friend class ENGINE_API FResourceUploader;
};
