#include "pch.hpp"

#include "Renderwerk/Profiler/Profiler.hpp"

#include "client/TracyProfiler.hpp"

void FProfiler::Initialize()
{
#ifdef TRACY_MANUAL_LIFETIME
	tracy::StartupProfiler();
	while (!tracy::IsProfilerStarted())
	{
	}
#endif
	if (tracy::IsProfilerStarted())
	{
		tracy::GetProfiler().SetProgramName(RW_ENGINE_NAME " v" RW_ENGINE_FULL_VERSION RW_ENGINE_VERSION_SUFFIX);
	}
}

void FProfiler::Shutdown()
{
#ifdef TRACY_MANUAL_LIFETIME
	tracy::ShutdownProfiler();
#endif
}
