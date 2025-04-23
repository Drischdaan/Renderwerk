#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

#if defined(RW_KIND_CONSOLE_APP)
#	define LAUNCH_HEADER() int main()
#elif defined(RW_KIND_WINDOWED_APP)
#	include <Windows.h>
#	define LAUNCH_HEADER() int WINAPI WinMain(HINSTANCE InstanceHandle, HINSTANCE PrevInstanceHandle, PSTR CmdLine, int CmdShow)
#else
#	define LAUNCH_HEADER()
#endif

#define LAUNCH_ENGINE() \
	extern "C" { __declspec(dllexport) extern const uint32 D3D12SDKVersion = 615;} \
	extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; } \
	LAUNCH_HEADER() \
	{ \
		return Launch(); \
	}

[[nodiscard]] ENGINE_API int32 Launch();
