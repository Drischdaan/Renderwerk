#include "pch.h"

#include "Renderwerk/Scene/Entity.h"

FEntity::FEntity() = default;

FEntity::FEntity(FScene* OwningScene, const flecs::entity& InEntity)
	: OwningScene(OwningScene), Entity(InEntity)
{
}

FEntity::~FEntity() = default;

bool8 FEntity::IsValid() const
{
	return Entity.is_valid();
}

bool8 FEntity::IsAlive() const
{
	return Entity.is_alive();
}

void FEntity::Destroy() const
{
	Entity.destruct();
}
