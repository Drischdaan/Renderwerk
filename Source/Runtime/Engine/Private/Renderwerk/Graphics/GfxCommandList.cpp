#include "pch.hpp"

#include "Renderwerk/Graphics/GfxCommandList.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"

FGfxCommandList::FGfxCommandList(FGfxDevice* InGfxDevice, const D3D12_COMMAND_LIST_TYPE InType)
	: FGfxCommandList(InGfxDevice, InType, TEXT("UnnamedCommandList"))
{
}

FGfxCommandList::FGfxCommandList(FGfxDevice* InGfxDevice, const D3D12_COMMAND_LIST_TYPE InType, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName), Type(InType)
{
	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	HRESULT Result = NativeDevice->CreateCommandAllocator(Type, IID_PPV_ARGS(&CommandAllocator));
	RW_VERIFY_ID(Result);

	Result = NativeDevice->CreateCommandList(0, Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList));
	RW_VERIFY_ID(Result);

	Result = CommandList->Close();
	RW_VERIFY_ID(Result);
}

FGfxCommandList::~FGfxCommandList()
{
	CommandList.Reset();
	CommandAllocator.Reset();
}

void FGfxCommandList::Open() const
{
	HRESULT Result = CommandAllocator->Reset();
	RW_VERIFY_ID(Result);

	Result = CommandList->Reset(CommandAllocator.Get(), nullptr);
	RW_VERIFY_ID(Result);
}

void FGfxCommandList::Close() const
{
	const HRESULT Result = CommandList->Close();
	RW_VERIFY_ID(Result);
}

void FGfxCommandList::SetRenderTarget(const TRef<FGfxTexture>& Texture) const
{
	const D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = Texture->GetRTVDescriptorHandle().GetCPUHandle();
	CommandList->OMSetRenderTargets(1, &CPUHandle, true, nullptr);
}

void FGfxCommandList::ClearRenderTarget(const TRef<FGfxTexture>& Texture, const float32 Color[4]) const
{
	CommandList->ClearRenderTargetView(Texture->GetRTVDescriptorHandle().GetCPUHandle(), Color, 0, nullptr);
}

void FGfxCommandList::ResourceBarrier(const TRef<FGfxTexture>& Texture, const D3D12_RESOURCE_STATES NewResourceState) const
{
	ID3D12Resource2* NativeResource = Texture->GetNativeObject<ID3D12Resource2>(NativeObjectIds::D3D12_Resource);
	const CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(NativeResource, Texture->GetResourceState(), NewResourceState);
	CommandList->ResourceBarrier(1, &ResourceBarrier);
	Texture->SetResourceState(NewResourceState);
}

FNativeObject FGfxCommandList::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::D3D12_CommandList)
	{
		return CommandList.Get();
	}
	if (NativeObjectId == NativeObjectIds::D3D12_CommandAllocator)
	{
		return CommandAllocator.Get();
	}
	return IGfxDeviceChild::GetRawNativeObject(NativeObjectId);
}
