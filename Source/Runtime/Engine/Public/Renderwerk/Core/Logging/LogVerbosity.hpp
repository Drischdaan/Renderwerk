#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

enum class ENGINE_API ELogVerbosity : uint8
{
	Trace = 0,
	Debug,
	Info,
	Warning,
	Error,
	Critical,
	Off,
};
