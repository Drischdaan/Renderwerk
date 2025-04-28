#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/Containers/Queue.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"
#include "Renderwerk/Platform/Threading/ConditionVariable.hpp"
#include "Renderwerk/Platform/Threading/CriticalSection.hpp"
#include "Renderwerk/Platform/Threading/Thread.hpp"

// TODO: Make this configurable or based on the available cpu cores
#define RW_DEFAULT_JOB_WORKERS FORWARD(4)

class IJob;

class ENGINE_API FJobModule : public IEngineModule
{
private:
	struct FJobQueue
	{
		FCriticalSection QueueSection;
		TQueue<TRef<IJob>> Queue;
	};

public:
	FJobModule();
	~FJobModule() override = default;

	NON_COPY_MOVEABLE(FJobModule)

public:
	void QueueJob(const TRef<IJob>& Job);

private:
	void Initialize() override;
	void Shutdown() override;

	void WorkerFunc();

	[[nodiscard]] TRef<IJob> GetNextJob();

public:
	DEFINE_MODULE_NAME("Job")

private:
	TVector<TRef<FThread>> Workers;

	std::atomic<bool8> bIsRunning = true;

	FCriticalSection WorkerWaitSection;
	FConditionVariable WorkerWaitVariable;

	FJobQueue CriticalPriorityJobs;
	FJobQueue HighPriorityJobs;
	FJobQueue NormalPriorityJobs;
	FJobQueue LowPriorityJobs;

	FCriticalSection ActiveJobsSection;
	TVector<TRef<IJob>> ActiveJobs;
};
