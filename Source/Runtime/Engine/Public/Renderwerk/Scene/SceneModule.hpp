#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"

class FScene;

class ENGINE_API FSceneModule : public IEngineModule
{
public:
	FSceneModule();
	~FSceneModule() override = default;

	NON_COPY_MOVEABLE(FSceneModule)

public:
	void SetActiveScene(const TRef<FScene>& Scene);

public:
	[[nodiscard]] TRef<FScene> GetActiveScene() const { return ActiveScene; }

private:
	void Initialize() override;
	void Shutdown() override;

public:
	DEFINE_MODULE_NAME("Scene")

private:
	TRef<FScene> ActiveScene;
};
