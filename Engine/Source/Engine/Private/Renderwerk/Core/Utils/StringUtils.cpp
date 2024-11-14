#include "pch.h"

#include "Renderwerk/Core/Utils/StringUtils.h"

#include <WinString.h>

FString ConvertToNarrowString(const FWideString& WideString)
{
	const size64 WideLength = WideString.length();
	const size64 NarrowLength = WideCharToMultiByte(CP_UTF8, 0, WideString.c_str(), static_cast<int32>(WideLength), nullptr, 0, nullptr, nullptr);
	FString Narrow(NarrowLength, '\0');
	WideCharToMultiByte(CP_UTF8, 0, WideString.c_str(), static_cast<int32>(WideLength), Narrow.data(), static_cast<int32>(NarrowLength), nullptr, nullptr);
	Narrow[NarrowLength] = '\0';
	return Narrow;
}

FWideString ConvertToWideString(const FString& NarrowString)
{
	const size64 NarrowLength = NarrowString.length();
	const size64 WideLength = MultiByteToWideChar(CP_UTF8, 0, NarrowString.c_str(), static_cast<int32>(NarrowLength), nullptr, 0);
	FWideString Wide(WideLength, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, NarrowString.c_str(), static_cast<int32>(NarrowLength), Wide.data(), static_cast<int32>(WideLength));
	Wide[WideLength] = L'\0';
	return Wide;
}
