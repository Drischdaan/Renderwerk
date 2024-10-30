#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <source_location>

#include "Renderwerk/Core/Definitions/CompilerDefinitions.h"

#pragma region Primitive Types

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using float32 = float;
using float64 = double;

using byte = uint8;

using bool8 = bool;

using size64 = uint64;

STATIC_ASSERT(sizeof(uint8) == 1);
STATIC_ASSERT(sizeof(uint16) == 2);
STATIC_ASSERT(sizeof(uint32) == 4);
STATIC_ASSERT(sizeof(uint64) == 8);

STATIC_ASSERT(sizeof(int8) == 1);
STATIC_ASSERT(sizeof(int16) == 2);
STATIC_ASSERT(sizeof(int32) == 4);
STATIC_ASSERT(sizeof(int64) == 8);

STATIC_ASSERT(sizeof(float32) == 4);
STATIC_ASSERT(sizeof(float64) == 8);

STATIC_ASSERT(sizeof(byte) == 1);

STATIC_ASSERT(sizeof(bool8) == 1);

STATIC_ASSERT(sizeof(size64) == 8);

#pragma endregion

using FSourceLocation = std::source_location;

template <typename T>
using TFuture = std::future<T>;

template <typename T>
using TAtomic = std::atomic<T>;

template <typename T>
using TFunction = std::function<T>;

/**
 * @brief Utility macro to streamline binding of functions.
 */
#define BIND_STATIC(Function) &Function

/**
 * @brief Utility macro to streamline binding of functions.
 */
#define BIND_LAMBDA(Lambda) Lambda

#define BIND_MEMBER(Function) std::bind(&Function, this)
#define BIND_MEMBER_ONE(Function) std::bind(&Function, this, std::placeholders::_1)
#define BIND_MEMBER_TWO(Function) std::bind(&Function, this, std::placeholders::_1, std::placeholders::_2)
#define BIND_MEMBER_THREE(Function) std::bind(&Function, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND_MEMBER_FOUR(Function) std::bind(&Function, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define BIND_MEMBER_FIVE(Function) std::bind(&Function, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)
