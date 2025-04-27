#pragma once

#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

class ENGINE_API IGfxPipeline : public IGfxDeviceChild
{
public:
	explicit IGfxPipeline(FGfxDevice* InGfxDevice);
	IGfxPipeline(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	~IGfxPipeline() override;

	NON_COPY_MOVEABLE(IGfxPipeline)

public:
	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

protected:
	TComPtr<ID3D12PipelineState> PipelineState;
	TRef<FGfxRootSignature> RootSignature;
};
