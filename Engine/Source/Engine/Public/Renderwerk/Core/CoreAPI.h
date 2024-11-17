#pragma once

// []===================================================================[]
// The macro RW_LIBRARY_SHARED is a user defined macro that is used
// to determine if the library is being built as a shared library or as
// a static library. If it is defined the ENGINE_API macro will be set
// to DLL_EXPORT if RW_COMPILE_LIBRARY is defined, otherwise it will
// be set to DLL_IMPORT. If RW_LIBRARY_SHARED is not defined, the
// ENGINE_API macro will be set to nothing.
// []===================================================================[]
#ifdef RW_LIBRARY_SHARED
#	if RW_COMPILE_LIBRARY
#		define ENGINE_API __declspec(dllexport)
#	else
		#define ENGINE_API __declspec(dllimport)
#	endif
#else
#	define ENGINE_API
#endif
