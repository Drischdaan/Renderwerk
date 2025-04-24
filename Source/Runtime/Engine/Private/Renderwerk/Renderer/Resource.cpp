#include "pch.hpp"

#include "Renderwerk/Renderer/Resource.hpp"

#include "Renderwerk/Renderer/Device.hpp"

IResource::IResource(FDevice* InDevice)
	: Device(InDevice)
{
}

IResource::~IResource()
{
	Resource.Reset();
}

void IResource::ChangeResourceState(const D3D12_RESOURCE_STATES NewState)
{
	ResourceState = NewState;
}

uint64 IResource::GetVirtualAddress() const
{
	return Resource->GetGPUVirtualAddress();
}

void IResource::CreateResource(const D3D12_HEAP_PROPERTIES* HeapProperties)
{
	const HRESULT Result = Device->GetHandle()->CreateCommittedResource2(HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, ResourceState, nullptr, nullptr,
	                                                                     IID_PPV_ARGS(&Resource));
	RW_VERIFY_ID(Result);

	Device->GetHandle()->GetCopyableFootprints1(&ResourceDesc, 0, ResourceDesc.MipLevels, 0, nullptr, nullptr, nullptr, &AllocationSize);
}
