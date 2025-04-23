#pragma once

#include <format>
#include <sstream>
#include <string>
#include <Windows.h>

#include "mimalloc.h"

#ifndef RW_USE_WIDE_STRINGS
#	if defined(UNICODE) || defined(_UNICODE)
#		define RW_USE_WIDE_STRINGS
#	endif
#endif

#ifndef TEXT
#	ifdef RW_USE_WIDE_STRINGS
#		define TEXT_PASTE(Value) L##Value
#	else
#		define TEXT_PASTE(Value) ##Value
#	endif
#	define TEXT(Value) TEXT_PASTE(Value)
#endif

using FWideChar = wchar_t;
using FWideString = std::basic_string<FWideChar, std::char_traits<FWideChar>, mi_stl_allocator<FWideChar>>;
using FWideStringView = std::basic_string_view<FWideChar>;
using FWideStringStream = std::basic_stringstream<FWideChar, std::char_traits<FWideChar>, mi_stl_allocator<FWideChar>>;

template <typename... TArguments>
using TWideFormatString = std::wformat_string<TArguments...>;

using FAnsiChar = char;
using FAnsiString = std::basic_string<FAnsiChar, std::char_traits<FAnsiChar>, mi_stl_allocator<FAnsiChar>>;
using FAnsiStringView = std::basic_string_view<FAnsiChar>;
using FAnsiStringStream = std::basic_stringstream<FAnsiChar, std::char_traits<FAnsiChar>, mi_stl_allocator<FAnsiChar>>;

template <typename... TArguments>
using TAnsiFormatString = std::format_string<TArguments...>;

#ifdef RW_USE_WIDE_STRINGS
using FChar = FWideChar;
using FString = FWideString;
using FStringView = FWideStringView;
using FStringStream = FWideStringStream;

template <typename... TArguments>
using TFormatString = TWideFormatString<TArguments...>;
#else
using FChar = FAnsiChar;
using FString = FAnsiString;
using FStringView = FAnsiStringView;
using FStringStream = FAnsiStringStream;

template <typename... TArguments>
using TFormatString = FAnsiFormatString<TArguments...>;
#endif
