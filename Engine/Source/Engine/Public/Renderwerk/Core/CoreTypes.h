#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <source_location>

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using float32 = float;
using float64 = double;

using size64 = uint64;

using bool8 = bool;

using byte = uint8;

static_assert(sizeof(int8) == 1, "int8 size is not 1 byte");
static_assert(sizeof(int16) == 2, "int16 size is not 2 bytes");
static_assert(sizeof(int32) == 4, "int32 size is not 4 bytes");
static_assert(sizeof(int64) == 8, "int64 size is not 8 bytes");

static_assert(sizeof(uint8) == 1, "uint8 size is not 1 byte");
static_assert(sizeof(uint16) == 2, "uint16 size is not 2 bytes");
static_assert(sizeof(uint32) == 4, "uint32 size is not 4 bytes");
static_assert(sizeof(uint64) == 8, "uint64 size is not 8 bytes");

static_assert(sizeof(float32) == 4, "float32 size is not 4 bytes");
static_assert(sizeof(float64) == 8, "float64 size is not 8 bytes");

static_assert(sizeof(size64) == 8, "size64 size is not 8 bytes");

static_assert(sizeof(bool8) == 1, "bool8 size is not 1 byte");

static_assert(sizeof(byte) == 1, "byte size is not 1 byte");

template <typename T>
using TFunction = std::function<T>;

template <typename T>
using TOptional = std::optional<T>;
