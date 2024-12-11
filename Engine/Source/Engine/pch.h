#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <ranges>
#include <ranges>
#include <set>
#include <source_location>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <xutility>

#include <Windows.h>

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Debug/Profiling.h"
#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Launch/FlowControl.h"
#include "Renderwerk/Threading/BufferedContainer.h"
#include "Renderwerk/Threading/SyncPoint.h"
#include "Renderwerk/Threading/ThreadTypes.h"

// [] - Dependencies

#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = FStringView;
#define MAGIC_ENUM_USING_ALIAS_STRING      using string      = FString;
#include "magic_enum/magic_enum.hpp"
