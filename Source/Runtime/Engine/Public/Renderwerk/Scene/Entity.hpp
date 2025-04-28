#pragma once

#include <type_traits>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Scene/Scene.hpp"

using FEntityId = uint32;

class ENGINE_API FEntity
{
public:
	FEntity() = default;
	FEntity(FScene* InOwnerScene, FEntityId InEntityId);
	~FEntity() = default;

	DEFAULT_COPY_MOVEABLE(FEntity)

public:
	[[nodiscard]] bool8 IsValid() const;

public:
	template <typename... TComponents>
	[[nodiscard]] bool8 HasComponents()
	{
		return OwnerScene->Registry.all_of<TComponents...>(static_cast<entt::entity>(EntityId));
	}

	template <typename TComponent>
	[[nodiscard]] TComponent& GetComponent()
	{
		return OwnerScene->Registry.get<TComponent>(static_cast<entt::entity>(EntityId));
	}

	template <typename TComponent, typename... TArguments> requires std::is_constructible_v<TComponent, TArguments...>
	TComponent& AddComponent(TArguments&&... Arguments)
	{
		return OwnerScene->Registry.emplace<TComponent, TArguments...>(static_cast<entt::entity>(EntityId), std::forward<TArguments>(Arguments)...);
	}

	template <typename TComponent>
	void RemoveComponent()
	{
		OwnerScene->Registry.remove<TComponent>(static_cast<entt::entity>(EntityId));
	}

public:
	[[nodiscard]] FEntityId GetEntityId() const { return EntityId; }

private:
	FScene* OwnerScene = nullptr;
	FEntityId EntityId = 0;

	friend class ENGINE_API FScene;
};
