#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/Types/CoreTypes.h"

enum class RENDERWERK_API ELogVerbosity : uint8
{
	Fatal = 0,
	Error,
	Warn,
	Info,
	Debug,
	Trace,
};
