#include "pch.h"

#include "Renderwerk/Core/Types/DeletionQueue.h"

FDeletionQueue::~FDeletionQueue()
{
	Process();
}

void FDeletionQueue::Push(FDeletionFunction&& Function)
{
	Queue.push_front(std::move(Function));
}

void FDeletionQueue::Process()
{
	while (!Queue.empty())
	{
		FDeletionFunction Function = Queue.front();
		Queue.pop_front();
		Function();
	}
}
