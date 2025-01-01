#pragma once

#include "Renderwerk/Graphics/GraphicsAdapter.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12Common.h"

class ENGINE_API FDirectX12GraphicsAdapter : public IGraphicsAdapter
{
public:
	FDirectX12GraphicsAdapter(IGraphicsBackend* GraphicsBackend, const ComPtr<IDXGIAdapter4>& Adapter);
	~FDirectX12GraphicsAdapter() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FDirectX12GraphicsAdapter);

public:
	void Initialize() override;
	void Destroy() override;

private:
	ComPtr<IDXGIAdapter4> Adapter;
};
