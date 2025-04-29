#include "pch.hpp"

#include "Renderwerk/Renderer/RendererModule.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Graphics/GfxAdapter.hpp"
#include "Renderwerk/Graphics/GfxContext.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
#include "Renderwerk/Graphics/GfxSurface.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FRendererModule::FRendererModule()
	: IEngineModule(EEngineThreadAffinity::Render)
{
}

void FRendererModule::Initialize()
{
	PROFILE_FUNCTION();
	{
		constexpr FGfxContextDesc ContextDesc = {
#ifdef RW_CONFIG_DEBUG
			.bEnableDebugLayer = true,
#else
			.bEnableDebugLayer = false,
#endif
		};
		GfxContext = NewRef<FGfxContext>(ContextDesc);

		Adapter = GfxContext->GetSuitableAdapter();
		Adapter->PrintAdapterInfo();

		constexpr FGfxDeviceDesc DeviceDesc = {
#ifdef RW_CONFIG_DEBUG
			.bEnableDebugLayer = true,
			.bEnableGPUValidation = true,
#else
			.bEnableDebugLayer = false,
			.bEnableGPUValidation = false,
#endif
		};
		Device = Adapter->CreateDevice(DeviceDesc, TEXT("MainDevice"));

		Surface = Device->CreateSurface(GetEngine()->GetWindow());

		TickDelegateHandle = GetEngine()->GetRenderThreadTickDelegate().BindRaw(this, &FRendererModule::OnTick);
	}
	RW_LOG(Info, "Renderer module initialized");
}

void FRendererModule::Shutdown()
{
	{
		GetEngine()->GetRenderThreadTickDelegate().Unbind(TickDelegateHandle);

		Device->FlushGraphicsQueue();

		Surface.reset();
		Device.reset();
		Adapter.reset();
		GfxContext.reset();
	}
	RW_LOG(Info, "Renderer module shutdown");
}

void FRendererModule::OnTick() const
{
	Surface->Render();
}
