#pragma once

#include <format>
#include <type_traits>

#include "Renderwerk/Core/Containers/String.hpp"

class FStringUtilities
{
public:
	template <typename... TArguments>
	[[nodiscard]] static FAnsiString Format(TAnsiFormatString<TArguments...> FormatString, TArguments&&... Arguments)
	{
		return std::format<TArguments...>(FormatString, std::forward<TArguments>(Arguments)...).c_str();
	}

	template <typename... TArguments>
	[[nodiscard]] static FWideString Format(TWideFormatString<TArguments...> FormatString, TArguments&&... Arguments)
	{
		return std::format<TArguments...>(FormatString, std::forward<TArguments>(Arguments)...).c_str();
	}
};
