#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Types/Containers.h"

using FDeletionFunction = TFunction<void()>;

struct ENGINE_API FDeletionQueue
{
public:
	FDeletionQueue() = default;
	~FDeletionQueue();

	DEFINE_DEFAULT_COPY_AND_MOVE(FDeletionQueue);

public:
	void Push(FDeletionFunction&& Function);
	void Process();

private:
	TDeque<FDeletionFunction> Queue;
};
