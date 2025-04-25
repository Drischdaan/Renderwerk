#include "pch.hpp"

#include "Renderwerk/Profiler/Profiler.hpp"

#ifdef TRACY_MANUAL_LIFETIME
#include "client/TracyProfiler.hpp"
#endif

void FProfiler::Initialize()
{
#ifdef TRACY_MANUAL_LIFETIME
	tracy::StartupProfiler();
	while (!tracy::IsProfilerStarted())
	{
	}

	if (tracy::IsProfilerStarted())
	{
		tracy::GetProfiler().SetProgramName(RW_ENGINE_NAME " v" RW_ENGINE_FULL_VERSION RW_ENGINE_VERSION_SUFFIX);
	}
#endif
}

void FProfiler::Shutdown()
{
#ifdef TRACY_MANUAL_LIFETIME
	tracy::ShutdownProfiler();
#endif
}
