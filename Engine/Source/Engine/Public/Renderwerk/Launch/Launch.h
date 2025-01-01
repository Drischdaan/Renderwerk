#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#if RW_KIND_CONSOLE_APP
#	define ENTRYPOINT_HEADER int main(int32 ArgumentCount, FChar* Arguments[])
#	define ENTRYPOINT_ARGUMENT_COUNT ArgumentCount
#	define ENTRYPOINT_ARGUMENT_ARRAY Arguments
#elif RW_KIND_WINDOWED_APP
#	include <Windows.h>
#	define ENTRYPOINT_HEADER int WINAPI WinMain(HINSTANCE InstanceHandle, HINSTANCE PreviousInstanceHandle, PSTR CommandLine, int ShowCommand)
#	define ENTRYPOINT_ARGUMENT_COUNT __argc
#	define ENTRYPOINT_ARGUMENT_ARRAY __argv
#endif

struct ENGINE_API FLaunchParameters
{
	uint32 ArgumentCount;
	FChar** Arguments;
};

ENGINE_API int32 LaunchEngine(const FLaunchParameters& Parameters);

/**
 * Macro to define the entry point for the application.
 * Ensures correct engine initialization and shutdown.
 */
#define LAUNCH_RENDERWERK() \
	 \
	extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 614; } \
	extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; } \
	ENTRYPOINT_HEADER \
	{ \
		FLaunchParameters Parameters = {}; \
		Parameters.ArgumentCount = ENTRYPOINT_ARGUMENT_COUNT; \
		Parameters.Arguments = ENTRYPOINT_ARGUMENT_ARRAY; \
		return LaunchEngine(Parameters); \
	}
