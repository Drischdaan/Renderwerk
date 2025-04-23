#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/GraphicsContext.hpp"

class ENGINE_API FRendererModule : public IEngineModule
{
public:
	FRendererModule();
	~FRendererModule() override = default;

	DEFAULT_COPY_MOVEABLE(FRendererModule)

private:
	void Initialize() override;
	void Shutdown() override;

public:
	DEFINE_MODULE_NAME("Renderer")

private:
	TObjectHandle<FGraphicsContext> Context;

	TObjectHandle<FAdapter> Adapter;
	TObjectHandle<FDevice> Device;
};
