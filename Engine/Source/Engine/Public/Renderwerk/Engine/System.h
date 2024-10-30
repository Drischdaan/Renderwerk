#pragma once

#include "Renderwerk/Core/CoreAPI.h"

/**
 * @brief Interface for all systems in the engine.
 */
class RENDERWERK_API ISystem
{
public:
	ISystem() = default;
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
