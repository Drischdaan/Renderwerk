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
	LAUNCH_HEADER() \
	{ \
		return 0; \
	}

[[nodiscard]] ENGINE_API int32 Launch();
