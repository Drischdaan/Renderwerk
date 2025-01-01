#pragma once

#include "Renderwerk/Graphics/DirectX12/DirectX12Common.h"

#include "Renderwerk/Graphics/GraphicsBackend.h"

class ENGINE_API FDirectX12GraphicsBackend : public IGraphicsBackend
{
public:
	FDirectX12GraphicsBackend();
	~FDirectX12GraphicsBackend() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FDirectX12GraphicsBackend);

public:
	void Initialize(const FGraphicsBackendDesc& InDescription) override;
	void Destroy() override;

	[[nodiscard]] TVector<TSharedPtr<IGraphicsAdapter>> GetAvailableAdapters() override;
	[[nodiscard]] bool8 IsAdapterSuitable(const TSharedPtr<IGraphicsAdapter>& GraphicsAdapter) override;

	[[nodiscard]] TSharedPtr<IGraphicsWindowContext> CreateWindowContext() override;

public:
	[[nodiscard]] ComPtr<IDXGIFactory7> GetFactory() const { return Factory; }

private:
	ComPtr<IDXGIDebug1> DXGIDebug;
	ComPtr<ID3D12Debug6> D3DDebug;

	ComPtr<IDXGIFactory7> Factory;
};
