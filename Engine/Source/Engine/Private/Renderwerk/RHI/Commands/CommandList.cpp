#include "pch.h"

#include "Renderwerk/RHI/Commands/CommandList.h"

FCommandList::FCommandList(FDevice* InDevice, const ECommandListType InType, const TComPtr<ID3D12CommandAllocator>& Allocator)
	: IDeviceChild(TEXT("CommandList"), InDevice), Type(InType)
{
	FD3DResult CreateResult = GetDeviceHandle()->CreateCommandList(0, static_cast<D3D12_COMMAND_LIST_TYPE>(Type), Allocator.Get(), nullptr, IID_PPV_ARGS(&CommandList));
	D3D_CHECKM(CreateResult, "Failed to create command list");
}

FCommandList::~FCommandList()
{
	CommandList.Reset();
}

void FCommandList::Reset(const TComPtr<ID3D12CommandAllocator>& Allocator) const
{
	DEBUG_D3D_CHECKM(CommandList->Reset(Allocator.Get(), nullptr), "Failed to reset command list");
}

void FCommandList::Close() const
{
	DEBUG_D3D_CHECKM(CommandList->Close(), "Failed to close command list");
}
