#include "pch.hpp"

#include "Renderwerk/Renderer/Camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"

FCamera::FCamera()
{
}

FCamera::FCamera(const float32 InWidth, const float32 InHeight)
	: Width(InWidth), Height(InHeight)
{
	RecreateProjectionMatrix();
}

FCamera::FCamera(const float32 InWidth, const float32 InHeight, const float32 InFieldOfView, const float32 InNearPlane, const float32 InFarPlane)
	: Width(InWidth), Height(InHeight), FieldOfView(InFieldOfView), NearPlane(InNearPlane), FarPlane(InFarPlane)
{
	RecreateProjectionMatrix();
}

void FCamera::SetFieldOfView(const float32 NewFieldOfView)
{
	FieldOfView = NewFieldOfView;
	bIsDirty = true;
}

void FCamera::SetSize(const float32 NewWidth, const float32 NewHeight)
{
	Width = NewWidth;
	Height = NewHeight;
	bIsDirty = true;
}

void FCamera::SetNearPlane(const float32 NewNearPlane)
{
	NearPlane = NewNearPlane;
	bIsDirty = true;
}

void FCamera::SetFarPlane(const float32 NewFarPlane)
{
	FarPlane = NewFarPlane;
	bIsDirty = true;
}

void FCamera::Update()
{
	if (bIsDirty)
	{
		bIsDirty = false;
		RecreateProjectionMatrix();
	}
}

void FCamera::RecreateProjectionMatrix()
{
	const float32 FOVRadians = glm::radians(FieldOfView);
	ProjectionMatrix = glm::perspectiveLH(FOVRadians, Width / Height, NearPlane, FarPlane);
	ProjectionMatrix = glm::transpose(ProjectionMatrix);
}
