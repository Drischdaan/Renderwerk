#include "pch.hpp"

#include "Renderwerk/Scene/SceneModule.hpp"

#include "Renderwerk/Scene/Entity.hpp"
#include "Renderwerk/Scene/Scene.hpp"
#include "Renderwerk/Scene/Components/CameraComponent.hpp"
#include "Renderwerk/Scene/Components/MeshComponent.hpp"

FSceneModule::FSceneModule()
	: IEngineModule(EEngineThreadAffinity::Update)
{
}

void FSceneModule::SetActiveScene(const TRef<FScene>& Scene)
{
	if (ActiveScene)
	{
		ActiveScene->Unload();
	}
	ActiveScene = Scene;
	Scene->Load();
}

void FSceneModule::Initialize()
{
	SetActiveScene(NewRef<FScene>());
	{
		FEntity Entity = ActiveScene->CreateEntity("Test2");
		FMeshComponent& MeshComponent = Entity.AddComponent<FMeshComponent>();

		const TVector<FVertex> Vertices = {
			{.Position = {-1.0f, -1.0f, 0.5f}, .TextureCoordinates = {1.0f, 0.0f}},
			{.Position = {1.0f, -1.0f, 0.5f}, .TextureCoordinates = {0.0f, 1.0f}},
			{.Position = {-1.0f, 1.0f, 0.5f}, .TextureCoordinates = {0.0f, 0.0f}},
			{.Position = {1.0f, 1.0f, 0.5f}, .TextureCoordinates = {1.0f, 1.0f}},
			{.Position = {-1.0f, -1.0f, -0.5f}, .TextureCoordinates = {1.0f, 1.0f}},
			{.Position = {1.0f, -1.0f, -0.5f}, .TextureCoordinates = {1.0f, 1.0f}},
			{.Position = {-1.0f, 1.0f, -0.5f}, .TextureCoordinates = {1.0f, 1.0f}},
			{.Position = {1.0f, 1.0f, -0.5f}, .TextureCoordinates = {1.0f, 1.0f}},
		};
		MeshComponent.Vertices = Vertices;

		const TVector<uint32> Indices = {
			2, 6, 7,
			2, 3, 7,
			0, 4, 5,
			0, 1, 5,
			0, 2, 6,
			0, 4, 6,
			1, 3, 7,
			1, 5, 7,
			0, 2, 3,
			0, 1, 3,
			4, 6, 7,
			4, 5, 7
		};
		MeshComponent.Indices = Indices;
	}
	{
		FEntity Entity = ActiveScene->CreateEntity("Test");
		FMeshComponent& MeshComponent = Entity.AddComponent<FMeshComponent>();

		const TVector<FVertex> Vertices = {
			FVertex{.Position = {0.5f, -0.5f, 0.0f}, .TextureCoordinates = {1.0f, 1.0f}},
			FVertex{.Position = {-0.5f, -0.5f, 0.0f}, .TextureCoordinates = {1.0f, 1.0f}},
			FVertex{.Position = {0.0f, 0.5f, 0.0f}, .TextureCoordinates = {1.0f, 1.0f}},
		};
		MeshComponent.Vertices = Vertices;

		const TVector<uint32> Indices = {0, 1, 2};
		MeshComponent.Indices = Indices;
	}
	{
		FEntity Entity = ActiveScene->CreateEntity("Camera");
		FCameraComponent& CameraComponent = Entity.AddComponent<FCameraComponent>();
		CameraComponent.bIsMainCamera = true;
	}
}

void FSceneModule::Shutdown()
{
	if (ActiveScene)
	{
		ActiveScene->Unload();
	}
	ActiveScene.reset();
}
