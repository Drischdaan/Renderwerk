#pragma once

#include "Renderwerk/Core/CoreAPI.h"

#if RW_ENABLE_PROFILING
#	include "tracy/Tracy.hpp"
#endif

// If profiling don't use constexpr, otherwise use constexpr
#if RW_ENABLE_PROFILING
#	define PROFILER_CONSTEXPR
#else
#	define PROFILER_CONSTEXPR CONSTEXPR
#endif

class RENDERWERK_API FProfiler
{
public:
	NODISCARD PROFILER_CONSTEXPR static bool8 IsProfilerConnected()
	{
#if RW_ENABLE_PROFILING
		return tracy::IsProfilerStarted();
#else
		return false;
#endif
	}

private:
	static void Initialize();
	static void Shutdown();

private:
	friend RENDERWERK_API int32 LaunchRenderwerk();
};

#if RW_ENABLE_PROFILING
#	define PROFILER_MARK_FRAME_START() FrameMarkStart(nullptr)
#	define PROFILER_MARK_FRAME_END() FrameMarkEnd(nullptr)
#	define PROFILER_MARK_FUNCTION() ZoneScoped
#	define PROFILER_MARK_SCOPE(Name) ZoneScopedN(Name)
#	define PROFILER_SET_SCOPE_NAME(Format, ...) ZoneNameF(Format, __VA_ARGS__)
#	define PROFILER_SET_THREAD_NAME(Name) tracy::SetThreadName(Name)
#	define PROFILER_SET_THREAD_NAME_HINT(Name, Hint) tracy::SetThreadNameWithHint(Name, Hint)
#	define PROFILER_MARK_ALLOCATION(Pointer, Size) TracyAlloc(Pointer, Size)
#	define PROFILER_MARK_FREE(Pointer) TracyFree(Pointer)
#else
#	define PROFILER_MARK_FRAME_START() (void)0
#	define PROFILER_MARK_FRAME_END() (void)0
#	define PROFILER_MARK_FUNCTION() (void)0
#	define PROFILER_MARK_SCOPE(Name) (void)0
#	define PROFILER_SET_SCOPE_NAME(Format, ...) (void)0
#	define PROFILER_SET_THREAD_NAME(Name) (void)0
#	define PROFILER_SET_THREAD_NAME_HINT(Name, Hint) (void)0
#	define PROFILER_MARK_ALLOCATION(Pointer, Size) (void)0
#	define PROFILER_MARK_FREE(Pointer) (void)0
#endif
