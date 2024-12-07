#pragma once

#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Types/String.h"
#include "Renderwerk/Core/Utility/Hasher.h"

/**
 * A compile-time hashed name that can be used to identify objects.
 * @note This is a 32-bit hash that is generated at compile-time.
 */
struct FName
{
public:
	constexpr FName() = default;

	constexpr FName(const FStringView InName)
		: Hash(FHasher::HashString(InName))
	{
	}

	constexpr FName(const uint32 InHash)
		: Hash(InHash)
	{
	}

	DEFINE_DEFAULT_COPY_AND_MOVE(FName);

public:
	constexpr bool8 operator==(const FName& Other) const { return Hash == Other.Hash; }
	constexpr bool8 operator!=(const FName& Other) const { return Hash != Other.Hash; }

	constexpr operator uint32() const { return Hash; }
	constexpr operator const uint32() const { return Hash; }
	constexpr operator uint32&() { return Hash; }
	constexpr operator const uint32&() const { return Hash; }
	constexpr operator uint32*() { return &Hash; }
	constexpr operator const uint32*() const { return &Hash; }

private:
	uint32 Hash = 0;
};

template <>
struct std::hash<FName>
{
	size64 operator()(const FName& InName) const noexcept
	{
		return std::hash<uint32>()(InName);
	}
};
