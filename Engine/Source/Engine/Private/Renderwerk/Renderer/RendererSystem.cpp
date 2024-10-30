#include "pch.h"

#include "Renderwerk/Renderer/RendererSystem.h"

#include "Renderwerk/RHI/Adapter.h"
#include "Renderwerk/RHI/RHI.h"

DEFINE_LOG_CATEGORY(LogRenderer);

FRendererSystem::FRendererSystem()
{
	FRHIDesc Description = {};
#if RW_CONFIG_DEBUG
	Description.bEnableDebugLayer = true;
	Description.bEnableValidationLayer = true;
#elif RW_CONFIG_DEVELOPMENT
	Description.bEnableDebugLayer = true;
#endif
	RHI = IRHI::Create(ERHIType::D3D12, Description);

	TVector<TSharedPtr<IAdapter>> Adapters = RHI->QueryAdapters();
	RW_LOG(LogRenderer, Info, "Available Adapters:");
	for (TSharedPtr<IAdapter> Adapter : Adapters)
	{
		RW_LOG(LogRenderer, Info, "\t- Adapter{}:", Adapter->GetIndex());
		RW_LOG(LogRenderer, Info, "\t\t- Name: {}", Adapter->GetName());
		RW_LOG(LogRenderer, Info, "\t\t- Type: {}", ToString(Adapter->GetType()));
		RW_LOG(LogRenderer, Info, "\t\t- Vendor: {}", ToString(Adapter->GetVendor()));
		RW_LOG(LogRenderer, Info, "\t\t- Video Memory: {}", Adapter->GetCapabilities().AvailableVideoMemory);
		RW_LOG(LogRenderer, Info, "\t\t- Support Features:");
		for (FAdapterFeature Feature : Adapter->GetCapabilities().SupportedFeatures)
			RW_LOG(LogRenderer, Info, "\t\t\t- {}", ToString(Feature.Type));
	}
	// TODO: Select adapter based on some criteria
	TSharedPtr<IAdapter> Adapter = Adapters.at(0);
	RW_LOG(LogRenderer, Info, "Selected Adapter: Adapter{} ({})", Adapter->GetIndex(), Adapter->GetName());

	RW_LOG(LogRenderer, Info, "Renderer system initialized");
}

FRendererSystem::~FRendererSystem()
{
	RHI.reset();
}
