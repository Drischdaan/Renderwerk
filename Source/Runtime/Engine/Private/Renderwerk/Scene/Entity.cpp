#include "pch.hpp"

#include "Renderwerk/Scene/Entity.hpp"

FEntity::FEntity(FScene* InOwnerScene, const FEntityId InEntityId)
	: OwnerScene(InOwnerScene), EntityId(InEntityId)
{
}

bool8 FEntity::IsValid() const
{
	return OwnerScene != nullptr && EntityId != 0;
}
