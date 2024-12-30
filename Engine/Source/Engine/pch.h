#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <fstream>
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
// Microsoft what is this shit??????????
#ifdef CreateSemaphore
#	undef CreateSemaphore
#endif

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Debug/Profiling.h"
#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Launch/FlowControl.h"
#include "Renderwerk/Logging/LogManager.h"
#include "Renderwerk/Threading/BufferedContainer.h"
#include "Renderwerk/Threading/SyncPoint.h"
#include "Renderwerk/Threading/ThreadTypes.h"

// [] - Dependencies
