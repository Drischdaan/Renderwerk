#pragma once

#include "Renderwerk/RHI/RHICommon.h"

class RENDERWERK_API FCommandList : public IDeviceChild
{
public:
	FCommandList(FDevice* InDevice, ECommandListType InType, const TComPtr<ID3D12CommandAllocator>& Allocator);
	~FCommandList() override;

	DELETE_COPY_AND_MOVE(FCommandList);

public:
	void Reset(const TComPtr<ID3D12CommandAllocator>& Allocator) const;
	void Close() const;

public:
	NODISCARD TComPtr<ID3D12GraphicsCommandList10> GetHandle() const { return CommandList; }

	NODISCARD ECommandListType GetType() const { return Type; }

private:
	ECommandListType Type;

	TComPtr<ID3D12GraphicsCommandList10> CommandList;
};
