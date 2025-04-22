#pragma once

#include <format>
#include <iomanip>
#include <ios>
#include <random>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/String.hpp"
#include "Renderwerk/Core/Misc/HashUtility.hpp"

struct ENGINE_API FGuid
{
public:
	constexpr FGuid() = default;

	constexpr FGuid(const uint64 InHigh, const uint64 InLow)
		: High(InHigh), Low(InLow)
	{
	}

	constexpr ~FGuid() = default;

	DEFAULT_COPY_MOVEABLE_PREFIX(FGuid, constexpr)

public:
	[[nodiscard]] constexpr bool8 IsValid() const
	{
		return High != 0 && Low != 0;
	}

	[[nodiscard]] FString ToString() const
	{
		FStringStream StringStream;
		StringStream << std::hex << std::setfill(TEXT('0'));
		StringStream << std::setw(8) << ((High >> 32) & 0xFFFFFFFF) << TEXT("-");
		StringStream << std::setw(4) << ((High >> 16) & 0xFFFF) << TEXT("-");
		StringStream << std::setw(4) << (High & 0xFFFF) << TEXT("-");
		StringStream << std::setw(4) << ((Low >> 48) & 0xFFFF) << TEXT("-");
		StringStream << std::setw(12) << (Low & 0xFFFFFFFFFFFF);
		return StringStream.str();
	}

public:
	[[nodiscard]] uint64 GetHigh() const { return High; }
	[[nodiscard]] uint64 GetLow() const { return Low; }

public:
	friend bool operator==(const FGuid& Lhs, const FGuid& Rhs)
	{
		return Lhs.High == Rhs.High && Lhs.Low == Rhs.Low;
	}

	friend bool operator!=(const FGuid& Lhs, const FGuid& Rhs)
	{
		return !(Lhs == Rhs);
	}

public:
	static FGuid Generate()
	{
		std::mt19937_64& GenerateRandom = GetRandomGenerator();
		return {GenerateRandom(), GenerateRandom()};
	}

	static FGuid Parse(const FString& GuidString)
	{
		FString String = GuidString;
		std::erase(String, TEXT('-'));
		if (String.length() != 32)
		{
			return {};
		}
		uint64_t High = 0;
		uint64_t Low = 0;
		FStringStream StringStreamHigh(String.substr(0, 16));
		StringStreamHigh >> std::hex >> High;
		FStringStream StringStreamLow(String.substr(16, 16));
		StringStreamLow >> std::hex >> Low;
		return {High, Low};
	}

private:
	static std::mt19937_64& GetRandomGenerator()
	{
		static std::random_device RandomDevice;
		static std::mt19937_64 Generator(RandomDevice());
		return Generator;
	}

private:
	uint64 High = 0;
	uint64 Low = 0;
};

template <>
struct std::hash<FGuid>
{
	size_t operator()(const FGuid& Guid) const noexcept
	{
		return FHashUtility::CombineHash(Guid.GetHigh(), Guid.GetLow());
	}
};

template <>
struct std::formatter<FGuid, FChar>
{
	template <typename ParseContext>
	constexpr auto parse(ParseContext& Context)
	{
		return Context.begin();
	}

	template <typename FormatContext>
	auto format(const FGuid& Guid, FormatContext& Context) const
	{
		FString String = Guid.ToString();
		return std::copy(String.begin(), String.end(), Context.out());
	}
};
