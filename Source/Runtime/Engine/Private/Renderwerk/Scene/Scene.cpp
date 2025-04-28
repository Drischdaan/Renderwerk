#include "pch.hpp"

#include "Renderwerk/Scene/Scene.hpp"

#include "imgui.h"

#include "Renderwerk/Graphics/RenderPass/GfxImguiRenderPass.hpp"
#include "Renderwerk/Scene/Entity.hpp"
#include "Renderwerk/Scene/Components/CameraComponent.hpp"
#include "Renderwerk/Scene/Components/SceneMetadataComponent.hpp"
#include "Renderwerk/Scene/Components/TagComponent.hpp"
#include "Renderwerk/Scene/Components/TransformComponent.hpp"

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
	Entity.AddComponent<FTransformComponent>();
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
	{
		ImGui::Begin(Metadata.Name.c_str());
		const auto View = Registry.view<const FTagComponent, FTransformComponent>();
		View.each([this](const entt::entity& Entity, const FTagComponent& TagComponent, FTransformComponent& TransformComponent)
		{
			ImGui::Text("%s", TagComponent.Name.c_str());
			ImGui::PushID(("DestroyButton" + std::to_string(static_cast<uint32>(Entity))).c_str());
			if (ImGui::Button("Destroy"))
			{
				Registry.destroy(Entity);
			}
			ImGui::PopID();

			ImGui::Text("Translation");
			ImGui::PushID(("Translation" + std::to_string(static_cast<uint32>(Entity))).c_str());
			float32 translation[3] = {TransformComponent.Translation.x, TransformComponent.Translation.y, TransformComponent.Translation.z};
			if (ImGui::DragFloat3("", translation, 0.1f))
			{
				TransformComponent.Translation = {translation[0], translation[1], translation[2]};
			}
			ImGui::PopID();

			ImGui::Text("Rotation (degrees)");
			ImGui::PushID(("Rotation" + std::to_string(static_cast<uint32>(Entity))).c_str());
			const glm::vec3 eulerDegrees = glm::degrees(TransformComponent.GetEulerRotation());
			float rotation[3] = {eulerDegrees.x, eulerDegrees.y, eulerDegrees.z};
			if (ImGui::DragFloat3("", rotation, 1.0f))
			{
				const glm::vec3 eulerRadians = glm::radians(glm::vec3(rotation[0], rotation[1], rotation[2]));
				TransformComponent.SetRotationEuler(eulerRadians);
			}
			ImGui::PopID();

			ImGui::Spacing();

			ImGui::Text("Scale");
			ImGui::PushID(("Scale" + std::to_string(static_cast<uint32>(Entity))).c_str());
			float32 scale[3] = {TransformComponent.Scale.x, TransformComponent.Scale.y, TransformComponent.Scale.z};
			if (ImGui::DragFloat3("", scale, 0.1f))
			{
				TransformComponent.Scale = {scale[0], scale[1], scale[2]};
			}
			ImGui::PopID();

			ImGui::PushID(("Matrix" + std::to_string(static_cast<uint32>(Entity))).c_str());
			if (ImGui::CollapsingHeader("Transform Matrix"))
			{
				glm::mat4 matrix = TransformComponent.GetTransform();
				ImGui::Text("Matrix:");
				for (int i = 0; i < 4; i++)
				{
					ImGui::Text("%.2f %.2f %.2f %.2f",
					            matrix[0][i], matrix[1][i], matrix[2][i], matrix[3][i]);
				}
			}
			ImGui::PopID();
		});
		ImGui::End();
	}
	{
		ImGui::Begin("Camera");
		{
			const auto View = Registry.view<FCameraComponent>();
			View.each([this](FCameraComponent& CameraComponent)
			{
				float32 FieldOfView = CameraComponent.Camera.GetFieldOfView();
				if (ImGui::SliderFloat("FieldOfView", &FieldOfView, 1.0f, 120.0f))
				{
					CameraComponent.Camera.SetFieldOfView(FieldOfView);
				}
			});
		}
		ImGui::End();
	}
}
