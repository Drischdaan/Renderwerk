#include "pch.hpp"

#include "Renderwerk/Job/JobModule.hpp"

#include "Renderwerk/Job/Job.hpp"
#include "Renderwerk/Platform/Threading/ScopedLock.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FJobModule::FJobModule()
	: IEngineModule(EEngineThreadAffinity::Main)
{
}

void FJobModule::QueueJob(const TRef<IJob>& Job)
{
	PROFILE_FUNCTION();
	{
		FScopedLock ActiveLock(&ActiveJobsSection);
		ActiveJobs.push_back(Job);
	}
	FJobQueue* TargetQueue = nullptr;
	switch (Job->GetPriority())
	{
	case EJobPriority::Low:
		TargetQueue = &LowPriorityJobs;
		break;
	case EJobPriority::Normal:
		TargetQueue = &NormalPriorityJobs;
		break;
	case EJobPriority::High:
		TargetQueue = &HighPriorityJobs;
		break;
	case EJobPriority::Critical:
		TargetQueue = &CriticalPriorityJobs;
		break;
	}
	{
		FScopedLock QueueLock(&TargetQueue->QueueSection);
		TargetQueue->Queue.push(Job);
	}
	WorkerWaitVariable.Notify();
}

void FJobModule::Initialize()
{
	Workers.reserve(RW_DEFAULT_JOB_WORKERS);
	for (uint32 Index = 0; Index < RW_DEFAULT_JOB_WORKERS; ++Index)
	{
		Workers.push_back(NewRef<FThread>([this]([[maybe_unused]] void* UserData) -> int32
		{
			WorkerFunc();
			return 0;
		}));
		Workers.at(Index)->Start();
	}
}

void FJobModule::Shutdown()
{
	bIsRunning = false;
	WorkerWaitVariable.NotifyAll();
	for (const TRef<FThread>& Worker : Workers)
	{
		Worker->Join();
	}
	Workers.clear();
}

void FJobModule::WorkerFunc()
{
	PROFILER_SET_THREAD_NAME_HINT("WorkerThread", 1);
	while (bIsRunning)
	{
		if (TRef<IJob> Job = GetNextJob())
		{
			Job->Execute();
			Job->MarkAsFinished();

			FScopedLock Lock(&ActiveJobsSection);
			const auto Iterator = std::ranges::find(ActiveJobs, Job);
			if (Iterator != ActiveJobs.end())
			{
				ActiveJobs.erase(Iterator);
			}
			continue;
		}
		FScopedLock Lock(&WorkerWaitSection);
		if (bIsRunning)
		{
			RW_VERIFY(WorkerWaitVariable.Wait(&WorkerWaitSection));
		}
	}
}

TRef<IJob> FJobModule::GetNextJob()
{
	auto CheckQueueForJob = [this](FJobQueue& JobQueue) -> TRef<IJob>
	{
		FScopedLock Lock(&JobQueue.QueueSection);
		if (!JobQueue.Queue.empty())
		{
			TRef<IJob> Job = JobQueue.Queue.front();
			JobQueue.Queue.pop();
			return Job;
		}
		return nullptr;
	};
	if (TRef<IJob> Job = CheckQueueForJob(CriticalPriorityJobs))
	{
		return Job;
	}
	if (TRef<IJob> Job = CheckQueueForJob(HighPriorityJobs))
	{
		return Job;
	}
	if (TRef<IJob> Job = CheckQueueForJob(NormalPriorityJobs))
	{
		return Job;
	}
	if (TRef<IJob> Job = CheckQueueForJob(LowPriorityJobs))
	{
		return Job;
	}
	return nullptr;
}
