#include "pch.h"

#include "Renderwerk/Scene/Scene.h"

#include "Renderwerk/Scene/Components.h"

FScene::FScene(const FString& InName)
	: Name(InName)
{
}

FScene::~FScene() = default;

void FScene::Initialize()
{
}

void FScene::Destroy()
{
	World.reset();
}

FEntity FScene::CreateEntity()
{
	FEntity Entity = {this, World.entity()};
	Entity.AddComponent<FGuidComponent>();
	RW_LOG(LogDefault, Info, "Created entity: {}", Entity.GetEntityId());
	return Entity;
}
