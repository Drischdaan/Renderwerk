#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

enum class ENGINE_API EGraphicsApiType : uint8
{
	None = 0,
	Vulkan,
	// DirectX12,
};

class ENGINE_API IGraphicsApi
{
public:
	IGraphicsApi(EGraphicsApiType InType);
	virtual ~IGraphicsApi();

	DELETE_COPY_AND_MOVE(IGraphicsApi);

public:
	[[nodiscard]] EGraphicsApiType GetType() const { return Type; }

private:
	static TUniquePtr<IGraphicsApi> NewInstance(EGraphicsApiType Type);

private:
	EGraphicsApiType Type;

	friend class ENGINE_API FRenderer;
};
