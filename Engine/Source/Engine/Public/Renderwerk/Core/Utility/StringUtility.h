#pragma once

namespace StringUtility
{
	[[nodiscard]] inline const char* ToNarrow(const wchar_t* InWide)
	{
		const size_t WideLength = wcslen(InWide);
		const size_t NarrowLength = WideCharToMultiByte(CP_UTF8, 0, InWide, static_cast<int32>(WideLength), nullptr, 0, nullptr, nullptr);
		char* Narrow = new char[NarrowLength + 1];
		WideCharToMultiByte(CP_UTF8, 0, InWide, static_cast<int32>(WideLength), Narrow, static_cast<int32>(NarrowLength), nullptr, nullptr);
		Narrow[NarrowLength] = '\0';
		return Narrow;
	}

	[[nodiscard]] inline const wchar_t* ToWide(const char* InNarrow)
	{
		const size_t NarrowLength = strlen(InNarrow);
		const size_t WideLength = MultiByteToWideChar(CP_UTF8, 0, InNarrow, static_cast<int32>(NarrowLength), nullptr, 0);
		wchar_t* Wide = new wchar_t[WideLength + 1];
		MultiByteToWideChar(CP_UTF8, 0, InNarrow, static_cast<int32>(NarrowLength), Wide, static_cast<int32>(WideLength));
		Wide[WideLength] = L'\0';
		return Wide;
	}
}
