#include "pch.hpp"

#include "Renderwerk/Graphics/Pipeline/GfxPipeline.hpp"

#include "Renderwerk/Graphics/GfxRootSignature.hpp"

IGfxPipeline::IGfxPipeline(FGfxDevice* InGfxDevice)
	: IGfxPipeline(InGfxDevice, TEXT("UnnamedPipeline"))
{
}

IGfxPipeline::IGfxPipeline(FGfxDevice* InGfxDevice, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName)
{
}

IGfxPipeline::~IGfxPipeline()
{
	RootSignature.reset();
	PipelineState.Reset();
}

FNativeObject IGfxPipeline::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::D3D12_PipelineState)
	{
		return PipelineState.Get();
	}
	if (NativeObjectId == NativeObjectIds::D3D12_RootSignature)
	{
		return RootSignature->GetRawNativeObject(NativeObjectId);
	}
	return IGfxDeviceChild::GetRawNativeObject(NativeObjectId);
}
