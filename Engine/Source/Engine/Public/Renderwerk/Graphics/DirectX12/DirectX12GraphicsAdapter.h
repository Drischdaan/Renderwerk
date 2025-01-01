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
	void Initialize(const TSharedPtr<IGraphicsWindowContext>& InGraphicsWindowContext) override;
	void Destroy() override;

public:
	[[nodiscard]] const CD3DX12FeatureSupport& GetFeatureSupport() const { return FeatureSupport; }

private:
	ComPtr<IDXGIAdapter4> Adapter;

	CD3DX12FeatureSupport FeatureSupport = {};
};
