#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

class ENGINE_API ILockable
{
public:
	ILockable() = default;
	virtual ~ILockable() = default;

	DEFAULT_COPY_MOVEABLE(ILockable)

public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;

	[[nodiscard]] virtual bool8 TryLock() = 0;

private:
};
