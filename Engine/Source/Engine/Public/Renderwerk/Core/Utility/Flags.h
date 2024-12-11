#pragma once

#include <type_traits>

template <typename TEnum> requires std::is_enum_v<TEnum>
struct TFlags
{
public:
	using TUnderlyingType = std::underlying_type_t<TEnum>;

public:
	constexpr TFlags()
		: Bits(0)
	{
	}

	constexpr TFlags(TEnum Value)
		: Bits(static_cast<TUnderlyingType>(Value))
	{
	}

	constexpr TFlags(TUnderlyingType Value)
		: Bits(Value)
	{
	}

	constexpr TFlags(const TFlags& Other) = default;
	constexpr TFlags(TFlags&& Other) = default;
	constexpr TFlags& operator=(const TFlags& Other) = default;
	constexpr TFlags& operator=(TFlags&& Other) = default;

	constexpr ~TFlags() = default;

public:
	constexpr TFlags operator&(const TFlags& Other) const { return TFlags(Bits & Other.Bits); }
	constexpr TFlags operator|(const TFlags& Other) const { return TFlags(Bits | Other.Bits); }
	constexpr TFlags operator^(const TFlags& Other) const { return TFlags(Bits ^ Other.Bits); }
	constexpr TFlags operator~() const { return TFlags(~Bits); }

	constexpr TFlags& operator|=(const TFlags& Other)
	{
		Bits |= Other.Bits;
		return *this;
	}

	constexpr TFlags& operator&=(const TFlags& Other)
	{
		Bits &= Other.Bits;
		return *this;
	}

	constexpr TFlags& operator^=(const TFlags& Other)
	{
		Bits ^= Other.Bits;
		return *this;
	}

	constexpr operator bool8() const { return Bits != 0; }
	constexpr explicit operator TUnderlyingType() const { return Bits; }

	constexpr bool8 operator==(const TFlags& Other) const { return Bits == Other.Bits; }
	constexpr bool8 operator!=(const TFlags& Other) const { return Bits != Other.Bits; }
	constexpr bool8 operator<=>(const TFlags& Other) const { return Bits <=> Other.Bits; }

private:
	TUnderlyingType Bits;
};

template <typename TEnum> requires std::is_enum_v<TEnum>
constexpr TFlags<TEnum> operator&(TEnum Value, const TFlags<TEnum>& Flags) noexcept
{
	return Flags.operator&(Value);
}

template <typename TEnum> requires std::is_enum_v<TEnum>
constexpr TFlags<TEnum> operator|(TEnum Value, const TFlags<TEnum>& Flags) noexcept
{
	return Flags.operator|(Value);
}

template <typename TEnum> requires std::is_enum_v<TEnum>
constexpr TFlags<TEnum> operator^(TEnum Value, const TFlags<TEnum>& Flags) noexcept
{
	return Flags.operator^(Value);
}

template <typename TEnum> requires std::is_enum_v<TEnum>
constexpr TFlags<TEnum> operator&(TEnum First, TEnum Second) noexcept
{
	return TFlags<TEnum>(First) & Second;
}

template <typename TEnum> requires std::is_enum_v<TEnum>
constexpr TFlags<TEnum> operator|(TEnum First, TEnum Second) noexcept
{
	return TFlags<TEnum>(First) | Second;
}

template <typename TEnum> requires std::is_enum_v<TEnum>
constexpr TFlags<TEnum> operator^(TEnum First, TEnum Second) noexcept
{
	return TFlags<TEnum>(First) ^ Second;
}

template <typename TEnum> requires std::is_enum_v<TEnum>
constexpr TFlags<TEnum> operator~(TEnum Value) noexcept
{
	return ~(TFlags<TEnum>(Value));
}
