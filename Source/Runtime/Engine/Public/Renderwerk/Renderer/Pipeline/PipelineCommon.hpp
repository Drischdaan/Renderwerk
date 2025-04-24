#pragma once

#include "Renderwerk/Renderer/GraphicsCommon.hpp"

enum class ENGINE_API ECullMode : uint8
{
	None = D3D12_CULL_MODE_NONE,
	Back = D3D12_CULL_MODE_BACK,
	Front = D3D12_CULL_MODE_FRONT,
};

enum class ENGINE_API EFillMode : uint8
{
	Solid = D3D12_FILL_MODE_SOLID,
	Wireframe = D3D12_FILL_MODE_WIREFRAME,
};

enum class ENGINE_API EDepthOperation : uint8
{
	None = D3D12_COMPARISON_FUNC_NONE,
	Greater = D3D12_COMPARISON_FUNC_GREATER,
	Less = D3D12_COMPARISON_FUNC_LESS,
	Equal = D3D12_COMPARISON_FUNC_EQUAL,
	LessEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,
};
