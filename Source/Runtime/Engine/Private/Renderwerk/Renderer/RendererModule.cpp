#include "pch.hpp"

#include "Renderwerk/Renderer/RendererModule.hpp"

#include "Renderwerk/Profiler/Profiler.hpp"

FRendererModule::FRendererModule()
	: IEngineModule(EEngineThreadAffinity::Render)
{
}

void FRendererModule::Initialize()
{
	PROFILE_FUNCTION();

	FGraphicsContextDesc ContextDesc = {
#ifdef RW_CONFIG_DEBUG
		.bEnableDebugLayer = true,
		.bEnableGPUValidation = true,
#else
		.bEnableDebugLayer = false,
		.bEnableGPUValidation = false,
#endif
	};
	Context = NewObjectHandle<FGraphicsContext>(ContextDesc);

	Adapter = Context->GetSuitableAdapter();
	RW_VERIFY_MSG(Adapter, "Unable to find suitable adapter");
	RW_LOG(Info, "Using adapter:");
	RW_LOG(Info, "\t- Name: {}", Adapter->GetName());
	RW_LOG(Info, "\t- Vendor: {}", FAdapter::GetVendorString(Adapter->GetVendor()));
	RW_LOG(Info, "\t- VideoMemory: {}", FStringUtilities::FormatMemorySize(Adapter->GetDedicatedVideoMemory()));
	RW_LOG(Info, "\t- SystemMemory: {}", FStringUtilities::FormatMemorySize(Adapter->GetSharedSystemMemory()));
	RW_LOG(Info, "\t- FeatureLevel: {}", D3DUtility::ToString(Adapter->GetFeatureLevel()));
	RW_LOG(Info, "\t- ShaderModel: {}", D3DUtility::ToString(Adapter->GetShaderModel()));
	RW_LOG(Info, "\t- ResourceBinding: {}", D3DUtility::ToString(Adapter->GetResourceBindingTier()));
	RW_LOG(Info, "\t- Raytracing: {}", D3DUtility::ToString(Adapter->GetRaytracingTier()));
	RW_LOG(Info, "\t- MeshShader: {}", D3DUtility::ToString(Adapter->GetMeshShaderTier()));

	RW_LOG(Info, "Adapter Memory Stats:");
	{
		const FAdapterMemoryStats DedicatedStats = Adapter->QueryMemoryStats(EAdapterMemoryType::Dedicated);
		RW_LOG(Info, "\t- Dedicated:");
		RW_LOG(Info, "\t\t- Available: {}", FStringUtilities::FormatMemorySize(DedicatedStats.AvailableMemory));
		RW_LOG(Info, "\t\t- Usage: {}", FStringUtilities::FormatMemorySize(DedicatedStats.UsedMemory));

		const FAdapterMemoryStats SharedStats = Adapter->QueryMemoryStats(EAdapterMemoryType::Shared);
		RW_LOG(Info, "\t- Shared:");
		RW_LOG(Info, "\t\t- Available: {}", FStringUtilities::FormatMemorySize(SharedStats.AvailableMemory));
		RW_LOG(Info, "\t\t- Usage: {}", FStringUtilities::FormatMemorySize(SharedStats.UsedMemory));
	}

	constexpr FDeviceDesc DeviceDesc = {};
	Device = Adapter->CreateDevice(DeviceDesc);

	RW_LOG(Info, "Renderer module initialized");
}

void FRendererModule::Shutdown()
{
	Device.Reset();
	Adapter.Reset();
	Context.Reset();
	RW_LOG(Info, "Renderer module shutdown");
}
