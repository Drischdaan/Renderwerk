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

	friend struct std::formatter<FName>;
	friend struct std::formatter<FName, FWideChar>;
};

template <>
struct std::hash<FName>
{
	size64 operator()(const FName& InName) const noexcept
	{
		return std::hash<uint32>()(InName);
	}
};

template <>
struct std::formatter<FName, FAnsiChar> : std::formatter<FAnsiStringView>
{
	template <typename FormatContext>
	auto format(const FName& Name, FormatContext& Context) const
	{
		return std::formatter<FAnsiStringView>::format(std::to_string(Name.Hash), Context);
	}
};

template <>
struct std::formatter<FName, FWideChar>
{
	template <class TParseContext>
	static constexpr auto parse(TParseContext& Context)
	{
		return Context.begin();
	}


	template <typename TFormatContext>
	auto format(const FName& Name, TFormatContext& Context) const
	{
		return format_to(Context.out(), L"{0}", std::to_wstring(Name.Hash));
	}
};
