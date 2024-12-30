#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Scene/Entity.h"

#include <flecs.h>

template <typename... TComponents>
using TQuery = flecs::query<TComponents...>;

class ENGINE_API FScene
{
public:
	FScene(const FString& InName);
	~FScene();

	DELETE_COPY_AND_MOVE(FScene);

public:
	void Initialize();
	void Destroy();

	FEntity CreateEntity();

public:
	template <typename... TComponents>
	TQuery<TComponents...> Query() const
	{
		return World.query_builder<TComponents...>().build();
	}

public:
	[[nodiscard]] FString GetName() const { return Name; }

	[[nodiscard]] flecs::world GetWorld() const
	{
		return World;
	}

private:
	FString Name;

	flecs::world World;
};
