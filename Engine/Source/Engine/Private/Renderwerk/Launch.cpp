#include "pch.h"

#include "Renderwerk/Launch.h"

#include "Renderwerk/Debug/Profiler.h"
#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

void InitializeCore()
{
	PROFILER_MARK_FUNCTION();
	FLogManager::Initialize();
#if RW_ENABLE_PROFILING
	RW_LOG(LogDefault, Warn, "Profiling is enabled. This will have a impact on performance");
#endif
#ifdef TRACY_CALLSTACK
	RW_LOG(LogDefault, Warn, "Tracy callstack is enabled. This will have further impact on performance");
#endif	

	GPlatform = MakeShared<FPlatform>();
}

void GuardedMain()
{
	GEngine = MakeShared<FEngine>();
	GEngine->Run();
}

void ShutdownCore()
{
	GPlatform.reset();
	FLogManager::Shutdown();
}

int32 LaunchRenderwerk()
{
	int32 ExitCode = EXIT_SUCCESS;
	FProfiler::Initialize();
	InitializeCore();
	__try
	{
		GuardedMain();
	}
	__except (FExceptionHandling::Handler(GetExceptionInformation()))
	{
		ExitCode = EXIT_FAILURE;
	}
	ShutdownCore();
	FProfiler::Shutdown();
	return ExitCode;
}
