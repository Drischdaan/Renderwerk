#pragma once

#include "tracy/Tracy.hpp"

#define DEFINE_PROFILER_SCOPE(Name) const char* const Profile##Name = #Name
#define USE_PROFILER_SCOPE(Name) extern const char* const Profile##Name

#define PROFILE_FUNCTION() ZoneScoped
#define PROFILE_SCOPE(Name) ZoneScopedN(Name)
#define PROFILE_SCOPEC(Name, Color) ZoneScopedNC(Name, Color)

#define PROFILE_FRAME() FrameMark
#define PROFILE_SECONDARY_FRAME(Name) FrameMarkNamed(Name)

#define PROFILE_POINTER_ALLOCATION(Pointer, Size) TracySecureAlloc(Pointer, Size)
#define PROFILE_POINTER_FREE(Pointer) TracySecureFree(Pointer)

class ENGINE_API FProfiler
{
public:
	static void Initialize();
	static void Shutdown();
};
