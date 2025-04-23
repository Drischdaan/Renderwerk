#pragma once

#include <format>
#include <type_traits>
#include <Windows.h>

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

public:
	[[nodiscard]] static FAnsiString ConvertToAnsi(const FWideString& String)
	{
		if (String.empty())
		{
			return {};
		}
		const size64 SizeNeeded = WideCharToMultiByte(CP_UTF8, 0, String.data(),
		                                              static_cast<int32>(String.size()), nullptr, 0, nullptr, nullptr);
		FAnsiString AnsiString;
		AnsiString.resize(SizeNeeded);
		const size64 Converted = WideCharToMultiByte(CP_UTF8, 0, String.data(),
		                                             static_cast<int32>(String.size()), AnsiString.data(), static_cast<int32>(AnsiString.size()), nullptr, nullptr);
		if (Converted == 0)
		{
			return {};
		}
		return AnsiString;
	}

	[[nodiscard]] static FWideString ConvertToWide(const FAnsiString& String)
	{
		if (String.empty())
		{
			return {};
		}
		const size64 SizeNeeded = MultiByteToWideChar(CP_UTF8, 0, String.data(),
		                                              static_cast<int32>(String.size()), nullptr, 0);
		FWideString WideString;
		WideString.resize(SizeNeeded);
		const size64 Converted = MultiByteToWideChar(CP_UTF8, 0, String.data(),
		                                             static_cast<int32>(String.size()), WideString.data(), static_cast<int32>(WideString.size()));
		if (Converted == 0)
		{
			return {};
		}
		return WideString;
	}
};
