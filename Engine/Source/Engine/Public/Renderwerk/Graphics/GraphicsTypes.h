#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

/**
 * Graphics feature types like ray tracing, mesh shaders, etc.
 */
enum class ENGINE_API EGraphicsFeatureType : uint8
{
	None = 0,
	MeshShaders,
	RayTracing,
	VariableRateShading,
};
