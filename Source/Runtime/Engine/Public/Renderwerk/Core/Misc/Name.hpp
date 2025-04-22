#pragma once

#include <format>
#include <type_traits>

#include "Renderwerk/Core/Containers/String.hpp"
#include "Renderwerk/Core/Misc/HashUtility.hpp"

struct ENGINE_API FName
{
public:
	constexpr FName() = default;

	constexpr FName(const FStringView InName)
		: Hash(FHashUtility::HashString(InName))
	{
	}

	constexpr FName(const uint32 InHash)
		: Hash(InHash)
	{
	}

	constexpr ~FName() = default;

	constexpr FName(const FName&) = default;
	constexpr FName& operator=(const FName&) = default;
	constexpr FName(FName&&) = default;
	constexpr FName& operator=(FName&&) = default;

public:
	[[nodiscard]] constexpr bool8 IsValid() const { return Hash != 0; }

public:
	constexpr bool8 operator==(const FName& Other) const { return Hash == Other.Hash; }
	constexpr bool8 operator!=(const FName& Other) const { return Hash != Other.Hash; }
	constexpr bool8 operator<(const FName& Other) const { return Hash < Other.Hash; }
	constexpr bool8 operator>(const FName& Other) const { return Hash > Other.Hash; }

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
struct std::less<FName>
{
	bool8 operator()(const FName& Lhs, const FName& Rhs) const noexcept
	{
		return Lhs < Rhs;
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
