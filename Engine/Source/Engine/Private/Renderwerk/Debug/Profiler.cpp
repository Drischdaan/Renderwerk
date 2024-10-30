#include "pch.h"

#include "Renderwerk/Debug/Profiler.h"

#if RW_ENABLE_PROFILING
#	include "tracy/Tracy.hpp"
#endif

void FProfiler::Initialize()
{
#if RW_ENABLE_PROFILING
	tracy::StartupProfiler();
	while (!tracy::IsProfilerStarted())
	{
	}
	TracySetProgramName(RW_ENGINE_NAME);
	tracy::SetThreadName("MainThread");
#endif
}

void FProfiler::Shutdown()
{
#if RW_ENABLE_PROFILING
	tracy::ShutdownProfiler();
#endif
}
