#pragma once

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"

class FGfxBuffer;

struct ENGINE_API FVertex
{
	float32 Position[3];
	float32 TextureCoordinates[2];
};

struct ENGINE_API FMeshComponent
{
	TVector<FVertex> Vertices;
	TVector<uint32> Indices;
	TRef<FGfxBuffer> VertexBuffer;
	TRef<FGfxBuffer> IndexBuffer;
	TRef<FGfxBuffer> ConstantBuffer;

	FMeshComponent() = default;

	FMeshComponent(const TVector<FVertex>& Vertices, const TVector<uint32>& Indices)
		: Vertices(Vertices), Indices(Indices)
	{
	}
};
