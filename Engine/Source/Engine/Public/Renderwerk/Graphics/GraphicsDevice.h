#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API IGraphicsDevice : public TGraphicsObject<IGraphicsDevice>, public IGraphicsBackendChild
{
public:
	IGraphicsDevice(IGraphicsBackend* InBackend)
		: IGraphicsBackendChild(InBackend)
	{
	}

	virtual ~IGraphicsDevice() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsDevice);

public:
	virtual void Initialize(const TSharedPtr<IGraphicsAdapter>& InGraphicsAdapter) = 0;
	virtual void Destroy() = 0;

protected:
	TSharedPtr<IGraphicsAdapter> GraphicsAdapter;
};
