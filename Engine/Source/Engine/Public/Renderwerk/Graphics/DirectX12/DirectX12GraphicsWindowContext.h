#pragma once

#include "Renderwerk/Graphics/GraphicsWindowContext.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12Common.h"

class ENGINE_API FDirectX12GraphicsWindowContext : public IGraphicsWindowContext
{
public:
	FDirectX12GraphicsWindowContext(IGraphicsBackend* InBackend);
	~FDirectX12GraphicsWindowContext() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FDirectX12GraphicsWindowContext);

public:
	void Initialize(const TSharedPtr<FWindow>& InWindow) override;
	void Destroy() override;

private:
};
