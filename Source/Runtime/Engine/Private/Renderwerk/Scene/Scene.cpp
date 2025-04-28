#include "pch.hpp"

#include "Renderwerk/Scene/Scene.hpp"

#include "imgui.h"

#include "Renderwerk/Graphics/RenderPass/GfxImguiRenderPass.hpp"
#include "Renderwerk/Scene/Entity.hpp"
#include "Renderwerk/Scene/Components/SceneMetadataComponent.hpp"
#include "Renderwerk/Scene/Components/TagComponent.hpp"

TMulticastDelegate<FEntity&> FScene::DeleteEntityDelegate;

FScene::FScene()
{
}

FScene::~FScene()
{
	Registry.clear();
}

void FScene::Load()
{
	SceneMetadataEntity = Registry.create();
	Registry.emplace<FSceneMetadataComponent>(SceneMetadataEntity, FSceneMetadataComponent{.Name = "UnnamedScene"});

	ImguiDelegateHandle = FGfxImguiRenderPass::GetImguiRenderDelegate().BindRaw(this, &FScene::OnImguiRender);
}

void FScene::Unload() const
{
	FGfxImguiRenderPass::GetImguiRenderDelegate().Unbind(ImguiDelegateHandle);
}

FEntity FScene::CreateEntity(const FAnsiString& Name)
{
	FEntity Entity = {this, static_cast<FEntityId>(Registry.create())};
	Entity.AddComponent<FTagComponent>(Name);
	return Entity;
}

void FScene::DeleteEntity(FEntity& Entity)
{
	DeleteEntityDelegate.Broadcast(Entity);
	Registry.destroy(static_cast<entt::entity>(Entity.GetEntityId()));
	Entity.OwnerScene = nullptr;
	Entity.EntityId = 0;
}

FSceneMetadataComponent FScene::GetSceneMetadata()
{
	return Registry.get<FSceneMetadataComponent>(SceneMetadataEntity);
}

void FScene::OnImguiRender()
{
	const FSceneMetadataComponent Metadata = GetSceneMetadata();
	ImGui::Begin(Metadata.Name.c_str());
	const auto View = Registry.view<const FTagComponent>();
	View.each([this](const entt::entity& Entity, const FTagComponent& TagComponent)
	{
		ImGui::Text("%s", TagComponent.Name.c_str());
		ImGui::PushID(std::to_string(static_cast<uint32>(Entity)).c_str());
		if (ImGui::Button("Destroy"))
		{
			Registry.destroy(Entity);
		}
		ImGui::PopID();
	});
	ImGui::End();
}
