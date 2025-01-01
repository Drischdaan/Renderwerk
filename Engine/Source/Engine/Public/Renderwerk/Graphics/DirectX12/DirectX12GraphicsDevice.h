#pragma once

#include "Renderwerk/Graphics/GraphicsDevice.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12Common.h"

class ENGINE_API FDirectX12GraphicsDevice : public IGraphicsDevice
{
public:
	FDirectX12GraphicsDevice(IGraphicsBackend* InBackend);
	~FDirectX12GraphicsDevice() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FDirectX12GraphicsDevice);

public:
	void Initialize(const TSharedPtr<IGraphicsAdapter>& InGraphicsAdapter) override;
	void Destroy() override;

private:
	ComPtr<ID3D12Device14> Device;
};
