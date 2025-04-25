#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"

#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

#define RW_DEFAULT_FRAME_COUNT FORWARD(3)

class ENGINE_API FRendererModule : public IEngineModule
{
public:
	FRendererModule();
	~FRendererModule() override = default;

	DEFAULT_COPY_MOVEABLE(FRendererModule)

private:
	void Initialize() override;
	void Shutdown() override;

	void OnTick() const;

public:
	DEFINE_MODULE_NAME("Renderer")

private:
	TRef<FGfxContext> GfxContext;

	TRef<FGfxAdapter> Adapter;
	TRef<FGfxDevice> Device;

	TRef<FGfxSwapchain> Swapchain;

	FDelegateHandle TickDelegateHandle;
};
