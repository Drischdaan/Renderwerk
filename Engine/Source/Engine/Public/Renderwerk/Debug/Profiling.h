#pragma once

#if RW_ENABLE_PROFILING
#	include <tracy/Tracy.hpp>
#	define START_PROFILER() tracy::StartupProfiler(); while (!tracy::IsProfilerStarted()) {} TracySetProgramName(RW_ENGINE_NAME)
#	define SHUTDOWN_PROFILER() tracy::ShutdownProfiler()
#	define PROFILE_SCOPE(Name) ZoneScopedN(Name)
#	define PROFILE_FUNCTION() ZoneScoped
#	define PROFILE_FRAME() FrameMark
#	define PROFILE_SECONDARY_FRAME(Name) FrameMarkNamed(Name)
#	define PROFILER_SET_THREAD_NAME(Name) tracy::SetThreadName(Name)
#	define PROFILER_MARK_ALLOCATION(Pointer, Size) TracyAlloc(Pointer, Size)
#	define PROFILER_MARK_FREE(Pointer) TracyFree(Pointer)
#else
#	define START_PROFILER()
#	define SHUTDOWN_PROFILER()
#	define PROFILE_SCOPE(Name)
#	define PROFILE_FUNCTION()
#	define PROFILE_FRAME()
#	define PROFILE_SECONDARY_FRAME(Name)
#	define PROFILER_SET_THREAD_NAME(Name)
#	define PROFILER_MARK_ALLOCATION(Pointer, Size)
#	define PROFILER_MARK_FREE(Pointer)
#endif
