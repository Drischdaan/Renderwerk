#pragma once

#include <atomic>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Delegates/Delegate.hpp"

enum class ENGINE_API EJobPriority : uint8
{
	Low = 0,
	Normal,
	High,
	Critical,
};

class ENGINE_API IJob
{
public:
	IJob(EJobPriority InPriority = EJobPriority::Normal);
	virtual ~IJob() = default;

	NON_COPY_MOVEABLE(IJob)

public:
	virtual void Execute() = 0;

public:
	void SetPriority(EJobPriority NewPriority);

public:
	[[nodiscard]] EJobPriority GetPriority() const { return Priority; }

	[[nodiscard]] bool8 IsFinished() const { return bIsFinished.load(); }
	[[nodiscard]] TDelegate<void>& GetCompletionDelegate() { return CompletionDelegate; }

private:
	void MarkAsFinished();

private:
	EJobPriority Priority;

	std::atomic<bool8> bIsFinished = false;
	TDelegate<void> CompletionDelegate;

	friend class ENGINE_API FJobModule;
};
