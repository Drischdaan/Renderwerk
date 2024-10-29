#pragma once

#include "Renderwerk/Core/CoreAPI.h"

class RENDERWERK_API ISystem
{
public:
	virtual ~ISystem() = default;

	DELETE_COPY_AND_MOVE(ISystem);

public:
	virtual void Initialize()
	{
	}

	virtual void Shutdown()
	{
	}

private:
};
