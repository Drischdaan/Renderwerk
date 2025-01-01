#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsFwd.h"

DECLARE_LOG_CHANNEL(LogGraphics);

enum class ENGINE_API EGraphicsBackendType : uint8
{
	None = 0,
	Vulkan = 1,
	DirectX12 = 2,
};

/**
 * Utility class for graphics objects, providing a way to cast to a specific implementation.
 */
template <typename T>
class ENGINE_API TGraphicsObject
{
public:
	template <typename TImplementation> requires std::is_base_of_v<T, TImplementation>
	TImplementation& As()
	{
		return static_cast<TImplementation&>(*this);
	}
};

class ENGINE_API IGraphicsBackendChild
{
public:
	IGraphicsBackendChild(IGraphicsBackend* InBackend)
		: Backend(InBackend)
	{
	}

	virtual ~IGraphicsBackendChild() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsBackendChild);

protected:
	IGraphicsBackend* Backend;
};
