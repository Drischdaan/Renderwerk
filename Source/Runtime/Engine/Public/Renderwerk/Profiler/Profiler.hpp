#pragma once

#ifdef TRACY_ENABLE
#	include "tracy/Tracy.hpp"
#	include "tracy/TracyD3D12.hpp"

#	define DEFINE_PROFILER_SCOPE(Name) const char* const Profile##Name = #Name
#	define USE_PROFILER_SCOPE(Name) extern const char* const Profile##Name

#	define PROFILE_FUNCTION() ZoneScoped
#	define PROFILE_SCOPE(Name) ZoneScopedN(Name)
#	define PROFILE_SCOPEC(Name, Color) ZoneScopedNC(Name, Color)

#	define PROFILER_SET_THREAD_NAME(Name) tracy::SetThreadName(Name)
#	define PROFILER_SET_THREAD_NAME_HINT(Name, Hint) tracy::SetThreadNameWithHint(Name, Hint)

#	define PROFILE_FRAME() FrameMark
#	define PROFILE_SECONDARY_FRAME(Name) FrameMarkNamed(Name)

#	define PROFILE_POINTER_ALLOCATION(Pointer, Size) TracySecureAlloc(Pointer, Size)
#	define PROFILE_POINTER_FREE(Pointer) TracySecureFree(Pointer)

#	define PROFILER_RENDER_CONTEXT(Device, Queue) tracy::CreateD3D12Context(Device, Queue)
#	define PROFILER_DESTROY_RENDER_CONTEXT(Context) tracy::DestroyD3D12Context(Context)

#	define PROFILE_RENDER_FRAME(Context) Context->NewFrame()
#	define PROFILE_RENDER_SCOPE(Context, CommandList, Name) TracyD3D12Zone(Context, CommandList->GetNativeObject<ID3D12GraphicsCommandList10>(NativeObjectIds::D3D12_CommandList), Name)
#	define PROFILE_RENDER_COLLECT(Context) Context->Collect()

using FProfilerRenderContext = TracyD3D12Ctx;
#else
#	define DEFINE_PROFILER_SCOPE(Name)
#	define USE_PROFILER_SCOPE(Name)

#	define PROFILE_FUNCTION()
#	define PROFILE_SCOPE(Name)
#	define PROFILE_SCOPEC(Name, Color)

#	define PROFILER_SET_THREAD_NAME(Name)
#	define PROFILER_SET_THREAD_NAME_HINT(Name, Hint)

#	define PROFILE_FRAME()
#	define PROFILE_SECONDARY_FRAME(Name)

#	define PROFILE_POINTER_ALLOCATION(Pointer, Size)
#	define PROFILE_POINTER_FREE(Pointer)

#	define PROFILER_RENDER_CONTEXT(Device, Queue) nullptr
#	define PROFILER_DESTROY_RENDER_CONTEXT(Context)

#	define PROFILE_RENDER_FRAME(Context)
#	define PROFILE_RENDER_SCOPE(Context, CommandList, Name)
#	define PROFILE_RENDER_COLLECT(Context)

using FProfilerRenderContext = void*;
#endif

class ENGINE_API FProfiler
{
public:
	static void Initialize();
	static void Shutdown();
};
