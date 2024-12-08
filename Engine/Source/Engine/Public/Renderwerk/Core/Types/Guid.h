#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Types/String.h"

#include <format>

enum : uint8
{
	INVALID_GUID_ID = 0
};

/**
 * A 64-bit unique identifier. This is a simple wrapper around a 64-bit integer, that is randomly generated.
 */
struct ENGINE_API FGuid
{
public:
	FGuid();

	constexpr FGuid(const uint64 InId)
		: Id(InId)
	{
	}

	constexpr ~FGuid() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FGuid);

public:
	[[nodiscard]] constexpr bool8 IsValid() const
	{
		return Id != INVALID_GUID_ID;
	}

public:
	constexpr operator bool8() const { return IsValid(); }

	constexpr bool8 operator==(const FGuid& Other) const { return Id == Other.Id; }
	constexpr bool8 operator!=(const FGuid& Other) const { return Id != Other.Id; }

	constexpr operator uint64() const { return Id; }
	constexpr operator const uint64() const { return Id; }
	constexpr operator uint64&() { return Id; }
	constexpr operator const uint64&() const { return Id; }
	constexpr operator uint64*() { return &Id; }
	constexpr operator const uint64*() const { return &Id; }

private:
	uint64 Id;

	friend struct std::formatter<FGuid>;
	friend struct std::formatter<FGuid, FWideChar>;
};

ENGINE_API extern const FGuid InvalidGuid;

template <>
struct std::hash<FGuid>
{
	size64 operator()(const FGuid& Guid) const noexcept
	{
		return std::hash<uint64>()(Guid);
	}
};

template <>
struct std::formatter<FGuid, FAnsiChar> : std::formatter<FAnsiStringView>
{
	template <typename FormatContext>
	auto format(const FGuid& Guid, FormatContext& Context) const
	{
		return std::formatter<FAnsiStringView>::format(std::to_string(Guid.Id), Context);
	}
};

template <>
struct std::formatter<FGuid, FWideChar>
{
	template <class TParseContext>
	static constexpr auto parse(TParseContext& Context)
	{
		return Context.begin();
	}


	template <typename TFormatContext>
	auto format(const FGuid& Guid, TFormatContext& Context) const
	{
		return format_to(Context.out(), L"{0}", std::to_wstring(Guid.Id));
	}
};
