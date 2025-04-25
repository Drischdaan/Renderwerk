#include "pch.hpp"

#include "Renderwerk/Renderer/RendererModule.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Graphics/GfxAdapter.hpp"
#include "Renderwerk/Graphics/GfxContext.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
#include "Renderwerk/Graphics/GfxSwapchain.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FRendererModule::FRendererModule()
	: IEngineModule(EEngineThreadAffinity::Render)
{
}

void FRendererModule::Initialize()
{
	PROFILE_FUNCTION();
	{
		FGfxContextDesc ContextDesc = {
			.bEnableDebugLayer = true,
			.bEnableGPUValidation = true
		};
		GfxContext = NewRef<FGfxContext>(ContextDesc);

		Adapter = GfxContext->GetSuitableAdapter();
		Adapter->PrintAdapterInfo();

		constexpr FGfxDeviceDesc DeviceDesc = {};
		Device = Adapter->CreateDevice(DeviceDesc, TEXT("MainDevice"));

		FGfxSwapchainDesc SwapchainDesc = {};
		SwapchainDesc.Window = GetEngine()->GetWindow();
		Swapchain = Device->CreateSwapchain(SwapchainDesc);

		TickDelegateHandle = GetEngine()->GetRenderThreadTickDelegate().BindRaw(this, &FRendererModule::OnTick);
	}
	RW_LOG(Info, "Renderer module initialized");
}

void FRendererModule::Shutdown()
{
	{
		GetEngine()->GetRenderThreadTickDelegate().Unbind(TickDelegateHandle);

		Swapchain.reset();
		Device.reset();
		Adapter.reset();
		GfxContext.reset();
	}
	RW_LOG(Info, "Renderer module shutdown");
}

void FRendererModule::OnTick() const
{
}
