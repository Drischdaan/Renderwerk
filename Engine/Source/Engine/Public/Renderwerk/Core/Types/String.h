﻿#pragma once

#include <string>

#include "Renderwerk/Core/Memory/Allocators.h"

#if defined(UNICODE) || defined(_UNICODE)
#	define RW_UNICODE_STRINGS
#else
#	define RW_ANSI_STRINGS
#endif

using FAnsiString = std::basic_string<char, std::char_traits<char>, FSTLAllocator<char>>;
using FAnsiStringView = std::string_view;
using FAnsiChar = char;

using FWideString = std::basic_string<wchar_t, std::char_traits<wchar_t>, FSTLAllocator<wchar_t>>;
using FWideStringView = std::wstring_view;
using FWideChar = wchar_t;

#ifdef RW_UNICODE_STRINGS
using FString = FWideString;
using FStringView = FWideStringView;
using FChar = FWideChar;
#else
using FString = FAnsiString;
using FStringView = FAnsiStringView;
using FChar = FAnsiChar;
#endif
