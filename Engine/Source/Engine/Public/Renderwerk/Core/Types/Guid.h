#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/CoreTypes.h"

#include <format>

#include "String.h"

/**
 * A 64-bit unique identifier. This is a simple wrapper around a 64-bit integer, that is randomly generated.
 */
struct ENGINE_API FGuid
{
public:
	FGuid();
	FGuid(uint64 InId);
	~FGuid();

	DEFINE_DEFAULT_COPY_AND_MOVE(FGuid);

public:
	[[nodiscard]] bool8 IsValid() const;

public:
	operator uint64() const { return Id; }
	operator const uint64() const { return Id; }

	operator uint64&() { return Id; }
	operator const uint64&() const { return Id; }

	operator uint64*() { return &Id; }
	operator const uint64*() const { return &Id; }

	operator bool8() const { return IsValid(); }

private:
	uint64 Id;
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
struct std::formatter<FGuid, char> : std::formatter<FAnsiStringView>
{
	template <typename FormatContext>
	auto format(const FGuid& Guid, FormatContext& Context) const
	{
		return std::formatter<FAnsiStringView>::format(std::to_string(Guid), Context);
	}
};

template <>
struct std::formatter<FGuid, wchar_t>
{
	template <class TParseContext>
	static constexpr auto parse(TParseContext& Context)
	{
		return Context.begin();
	}


	template <typename TFormatContext>
	auto format(const FGuid& Guid, TFormatContext& Context) const
	{
		return format_to(Context.out(), L"{0}", std::to_wstring(Guid));
	}
};
