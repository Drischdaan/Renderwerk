#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/Types/StringTypes.h"

ENGINE_API FString ConvertToNarrowString(const FWideString& WideString);
ENGINE_API FWideString ConvertToWideString(const FString& NarrowString);
