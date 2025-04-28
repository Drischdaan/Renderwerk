#include "pch.hpp"

#include "Renderwerk/Job/Job.hpp"

IJob::IJob(const EJobPriority InPriority)
	: Priority(InPriority)
{
}

void IJob::SetPriority(const EJobPriority NewPriority)
{
	Priority = NewPriority;
}

void IJob::MarkAsFinished()
{
	bIsFinished = true;
	CompletionDelegate.ExecuteIfBound();
}
