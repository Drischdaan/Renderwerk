#pragma once

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"

class FGfxBuffer;

struct ENGINE_API FVertex
{
	float32 Position[3];
	float32 Color[3];
};

struct ENGINE_API FMeshComponent
{
	TVector<FVertex> Vertices;
	TVector<uint32> Indices;
	TRef<FGfxBuffer> VertexBuffer;
	TRef<FGfxBuffer> IndexBuffer;
	TRef<FGfxBuffer> ConstantBuffer;
};
