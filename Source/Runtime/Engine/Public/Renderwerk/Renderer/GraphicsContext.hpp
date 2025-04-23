#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/Adapter.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

struct ENGINE_API FGraphicsContextDesc
{
	bool8 bEnableDebugLayer = false;
	bool8 bEnableGPUValidation = false;
};

class ENGINE_API FGraphicsContext : public TGraphicsObject<FGraphicsContext>
{
public:
	FGraphicsContext(const FGraphicsContextDesc& InDescription);
	~FGraphicsContext() override;

	NON_COPY_MOVEABLE(FGraphicsContext)

public:
	[[nodiscard]] TObjectHandle<FAdapter> GetSuitableAdapter();

public:
	[[nodiscard]] TObjectHandle<IDXGIFactory7> GetFactory() const { return Factory; }

private:
	FGraphicsContextDesc Description;

	TObjectHandle<IDXGIDebug1> DXGIDebug;
	TObjectHandle<ID3D12Debug6> D3D12Debug;

	TObjectHandle<IDXGIFactory7> Factory;
};
