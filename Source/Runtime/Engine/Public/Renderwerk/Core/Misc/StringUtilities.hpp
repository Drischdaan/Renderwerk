#pragma once

#include <format>
#include <type_traits>
#include <Windows.h>

#include "Renderwerk/Core/PrimitiveTypes.hpp"
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

public:
	[[nodiscard]] static FWideString FormatMemorySizeWide(const size64 Bytes)
	{
		const FWideChar* Units[] = {L"B", L"KB", L"MB", L"GB", L"TB", L"PB"};
		int32 UnitIndex = 0;
		float64 Size = static_cast<float64>(Bytes);
		while (Size >= 1024.0 && UnitIndex < 5)
		{
			Size /= 1024.0;
			UnitIndex++;
		}
		FWideChar Buffer[64];
		if (UnitIndex == 0 || Size == std::floor(Size))
		{
			(void)swprintf(Buffer, sizeof(Buffer) / sizeof(FWideChar), L"%.0f%ls", Size, Units[UnitIndex]);
		}
		else
		{
			(void)swprintf(Buffer, sizeof(Buffer) / sizeof(FWideChar), L"%.2f%ls", Size, Units[UnitIndex]);
		}
		return {Buffer};
	}

	[[nodiscard]] static FAnsiString FormatMemorySizeAnsi(const size64 Bytes)
	{
		const FAnsiChar* Units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
		int32 unitIndex = 0;
		float64 size = static_cast<float64>(Bytes);
		while (size >= 1024.0 && unitIndex < 5)
		{
			size /= 1024.0;
			unitIndex++;
		}
		FAnsiChar Buffer[64];
		if (unitIndex == 0 || size == std::floor(size))
		{
			(void)snprintf(Buffer, sizeof(Buffer), "%.0f%s", size, Units[unitIndex]);
		}
		else
		{
			(void)snprintf(Buffer, sizeof(Buffer), "%.2f%s", size, Units[unitIndex]);
		}
		return {Buffer};
	}

	[[nodiscard]] static FString FormatMemorySize(const size64 Bytes)
	{
#ifdef RW_USE_WIDE_STRINGS
		return FormatMemorySizeWide(Bytes);
#else
			return FormatMemorySizeAnsi(Bytes);
#endif
	}
};
