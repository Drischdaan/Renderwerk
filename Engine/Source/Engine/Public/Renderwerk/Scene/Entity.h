#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include <flecs.h>

class FScene;

using FEntityId = flecs::entity_t;

struct ENGINE_API FEntity
{
public:
	FEntity();
	FEntity(FScene* OwningScene, const flecs::entity& InEntity);
	~FEntity();

	DEFINE_DEFAULT_COPY_AND_MOVE(FEntity);

public:
	[[nodiscard]] bool8 IsValid() const;

	void Destroy() const;

public:
	[[nodiscard]] FEntityId GetEntityId() const { return Entity.id(); }

public:
	template <typename TComponent>
	[[nodiscard]] bool8 HasComponent() const
	{
		return Entity.has<TComponent>();
	}

	template <typename TComponent>
	void AddComponent() const
	{
		Entity.add<TComponent>();
	}

	template <typename TComponent>
	void EmplaceComponent(const TComponent& Component)
	{
		Entity.set<TComponent>(Component);
	}

	template <typename TComponent>
	[[nodiscard]] TComponent* GetComponent() const
	{
		return Entity.get<TComponent>();
	}

	template <typename TComponent>
	void RemoveComponent()
	{
		Entity.remove<TComponent>();
	}

private:
	FScene* OwningScene;
	flecs::entity Entity;
};
