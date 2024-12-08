#pragma once

// [] - Engine

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Launch/FlowControl.h"
#include "Renderwerk/Threading/BufferedContainer.h"
#include "Renderwerk/Threading/SyncPoint.h"
#include "Renderwerk/Threading/ThreadTypes.h"

// [] - Dependencies

#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = FStringView;
#define MAGIC_ENUM_USING_ALIAS_STRING      using string      = FString;
#include "magic_enum/magic_enum.hpp"
