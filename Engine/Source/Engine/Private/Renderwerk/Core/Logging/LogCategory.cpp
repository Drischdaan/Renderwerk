#include "pch.h"

#include "Renderwerk/Core/Logging/LogCategory.h"

ILogCategory::ILogCategory(const FAnsiString& InName, const ELogVerbosity InVerbosity)
	: Name(InName), Verbosity(InVerbosity)
{
}
