#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Renderwerk/Core/CoreAPI.hpp"

struct ENGINE_API FTransformComponent
{
	glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
	glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

private:
	glm::vec3 RotationEuler = {0.0f, 0.0f, 0.0f};
	glm::quat Rotation = {1.0f, 0.0f, 0.0f, 0.0f};

public:
	void SetRotationEuler(const glm::vec3& euler)
	{
		RotationEuler = euler;
		Rotation = glm::quat(RotationEuler);
	}

	[[nodiscard]] glm::quat GetQuaternionRotation() const
	{
		return Rotation;
	}

	[[nodiscard]] glm::vec3 GetEulerRotation() const
	{
		return RotationEuler;
	}

	[[nodiscard]] glm::mat4 GetTransform() const
	{
		const glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Translation) * glm::toMat4(Rotation) * glm::scale(glm::mat4(1.0f), Scale);
		return glm::transpose(Transform);
	}
};
