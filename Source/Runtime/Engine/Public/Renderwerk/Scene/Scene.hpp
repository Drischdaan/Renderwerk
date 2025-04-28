#pragma once

#include "Components/SceneMetadataComponent.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"

#include "entt/entity/registry.hpp"

#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"

class FEntity;

class ENGINE_API FScene
{
public:
	FScene();
	~FScene();

	NON_COPY_MOVEABLE(FScene)

public:
	void Load();
	void Unload() const;

	[[nodiscard]] FEntity CreateEntity(const FAnsiString& Name = "UnnamedEntity");
	void DeleteEntity(FEntity& Entity);

	[[nodiscard]] FSceneMetadataComponent GetSceneMetadata();

public:
	template <typename... TComponents>
	[[nodiscard]] auto CreateView()
	{
		return Registry.view<TComponents...>();
	}

public:
	[[nodiscard]] static TMulticastDelegate<FEntity&>& GetDeleteEntityDelegate() { return DeleteEntityDelegate; }

private:
	void OnImguiRender();

private:
	static TMulticastDelegate<FEntity&> DeleteEntityDelegate;

private:
	entt::registry Registry;
	entt::entity SceneMetadataEntity = {};

	FDelegateHandle ImguiDelegateHandle;

	friend class ENGINE_API FEntity;
};
