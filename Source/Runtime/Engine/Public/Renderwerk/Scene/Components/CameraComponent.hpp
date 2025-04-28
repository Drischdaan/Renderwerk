#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/Camera.hpp"

struct ENGINE_API FCameraComponent
{
	FCamera Camera;
	bool8 bIsMainCamera = false;
};
