#pragma once

// Wrapper around magic_enum to make it fit our coding standards

#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = FStringView;
#define MAGIC_ENUM_USING_ALIAS_STRING      using string      = FString;
#include "magic_enum/magic_enum.hpp"

template <typename TEnum> requires std::is_enum_v<TEnum>
[[nodiscard]] constexpr FStringView GetEnumValueName(TEnum Value)
{
	return magic_enum::enum_name<TEnum>(Value);
}

template <typename TEnum, magic_enum::detail::enum_subtype TSubType = magic_enum::detail::subtype_v<TEnum>> requires std::is_enum_v<TEnum>
[[nodiscard]] constexpr magic_enum::detail::names_t<TEnum, TSubType> GetEnumNames()
{
	return magic_enum::enum_names<TEnum, TSubType>();
}

template <typename TEnum, magic_enum::detail::enum_subtype TSubType = magic_enum::detail::subtype_v<TEnum>> requires std::is_enum_v<TEnum>
[[nodiscard]] constexpr magic_enum::detail::entries_t<TEnum, TSubType> GetEnumValueNamePair()
{
	return magic_enum::enum_entries<TEnum, TSubType>();
}
