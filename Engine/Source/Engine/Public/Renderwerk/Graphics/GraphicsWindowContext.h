#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API IGraphicsWindowContext : public TGraphicsObject<IGraphicsWindowContext>, public IGraphicsBackendChild
{
public:
	IGraphicsWindowContext(IGraphicsBackend* InBackend)
		: IGraphicsBackendChild(InBackend)
	{
	}

	virtual ~IGraphicsWindowContext() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsWindowContext);

public:
	virtual void Initialize(const TSharedPtr<FWindow>& InWindow) = 0;
	virtual void Destroy() = 0;

protected:
	TSharedPtr<FWindow> Window;
};
