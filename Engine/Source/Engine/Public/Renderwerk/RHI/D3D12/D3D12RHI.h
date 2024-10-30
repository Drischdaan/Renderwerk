#pragma once

#include "Renderwerk/RHI/RHI.h"
#include "Renderwerk/RHI/D3D12/D3D12Common.h"

DECLARE_LOG_CATEGORY(LogD3D12RHI, Trace);

class RENDERWERK_API FD3D12RHI : public IRHI
{
public:
	FD3D12RHI(const FRHIDesc& InDescription);
	~FD3D12RHI() override;

	DELETE_COPY_AND_MOVE(FD3D12RHI);

public:
	NODISCARD TVector<TSharedPtr<IAdapter>> QueryAdapters() override;

private:
	TComPtr<IDXGIDebug1> DXGIDebug;
	TComPtr<ID3D12Debug6> D3DDebug;

	TComPtr<IDXGIFactory7> Factory;
};
