#include "pch.hpp"

#include "Renderwerk/Graphics/GfxResource.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"

IGfxResource::IGfxResource(FGfxDevice* InGfxDevice)
	: IGfxResource(InGfxDevice, TEXT("UnnamedResource"))
{
}

IGfxResource::IGfxResource(FGfxDevice* InGfxDevice, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName)
{
}

IGfxResource::~IGfxResource()
{
	Resource.Reset();
}

FNativeObject IGfxResource::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::D3D12_Resource)
	{
		return Resource.Get();
	}
	return IGfxDeviceChild::GetRawNativeObject(NativeObjectId);
}

void IGfxResource::CreateResource(const D3D12_HEAP_PROPERTIES* HeapProperties)
{
	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	const HRESULT Result = NativeDevice->CreateCommittedResource2(HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, ResourceState, nullptr, nullptr,
	                                                              IID_PPV_ARGS(&Resource));
	RW_VERIFY_ID(Result);

	NativeDevice->GetCopyableFootprints1(&ResourceDesc, 0, ResourceDesc.MipLevels, 0, nullptr, nullptr, nullptr, &AllocationSize);
}
