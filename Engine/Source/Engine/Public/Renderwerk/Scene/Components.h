#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct ENGINE_API FGuidComponent
{
	FGuid Id;
};

struct ENGINE_API FTransformComponent
{
	glm::vec3 Position = glm::vec3(0.f);
	glm::vec3 Rotation = glm::vec3(0.f);
	glm::vec3 Scale = glm::vec3(1.f);
};
