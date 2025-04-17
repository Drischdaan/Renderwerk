#pragma once

#ifdef RW_LINK_MODULAR
#	ifdef RW_COMPILE_ENGINE_API
#		define ENGINE_API __declspec(dllexport)
#	else
#		define ENGINE_API __declspec(dllimport)
#	endif
#else
#	define ENGINE_API
#endif
