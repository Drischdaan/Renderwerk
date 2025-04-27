#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

struct ENGINE_API FGfxContextDesc
{
	bool8 bEnableDebugLayer = false;
	bool8 bEnableGPUValidation = false;
};

class ENGINE_API FGfxContext : public IGfxObject
{
public:
	FGfxContext(const FGfxContextDesc& InContextDesc);
	~FGfxContext() override;

	NON_COPY_MOVEABLE(FGfxContext)

public:
	[[nodiscard]] TRef<FGfxAdapter> GetSuitableAdapter();

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

private:
	[[nodiscard]] static uint8 CalculateAdapterScore(const TComPtr<IDXGIAdapter4>& Adapter);

private:
	FGfxContextDesc ContextDesc;

	TComPtr<IDXGIDebug1> DXGIDebug;
	TComPtr<ID3D12Debug6> D3D12Debug;

	TComPtr<IDXGIFactory7> Factory;
};
