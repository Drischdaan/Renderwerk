#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"

#include <glm/glm.hpp>

class ENGINE_API FCamera
{
public:
	FCamera();
	FCamera(float32 InWidth, float32 InHeight);
	FCamera(float32 InWidth, float32 InHeight, float32 InFieldOfView = 60.0f, float32 InNearPlane = 0.01f, float32 InFarPlane = 1000.0f);
	virtual ~FCamera() = default;

	DEFAULT_COPY_MOVEABLE(FCamera)

public:
	void SetFieldOfView(float32 NewFieldOfView);
	void SetSize(float32 NewWidth, float32 NewHeight);
	void SetNearPlane(float32 NewNearPlane);
	void SetFarPlane(float32 NewFarPlane);

	void Update();

public:
	[[nodiscard]] float32 GetFieldOfView() const { return FieldOfView; }
	[[nodiscard]] float32 GetWidth() const { return Width; }
	[[nodiscard]] float32 GetHeight() const { return Height; }
	[[nodiscard]] float32 GetNearPlane() const { return NearPlane; }
	[[nodiscard]] float32 GetFarPlane() const { return FarPlane; }

	[[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }

private:
	void RecreateProjectionMatrix();

private:
	float32 Width = 0.0f;
	float32 Height = 0.0f;
	float32 FieldOfView = 60.0f;
	float32 NearPlane = 0.01f;
	float32 FarPlane = 1000.0f;

	bool8 bIsDirty = false;

	glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
};
