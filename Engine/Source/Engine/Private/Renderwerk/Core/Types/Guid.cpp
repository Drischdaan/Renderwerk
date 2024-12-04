#include "pch.h"

#include "Renderwerk/Core/Types/Guid.h"

#include <objbase.h>

FGuid NewGuid()
{
	GUID Guid;
	CoCreateGuid(&Guid);
	return Guid;
}

FGuid NewGuid(FString String)
{
	FString Temp = std::move(String);
	if (!Temp.starts_with(TEXT("{")))
		Temp.insert(0, TEXT("{"));
	if (!Temp.ends_with(TEXT("}")))
		Temp.append(TEXT("}"));
	printf("Temp: %ls\n", Temp.c_str());
	FGuid Guid;
	CLSIDFromString(Temp.data(), &Guid); // TODO: Error handling
	return Guid;
}

bool8 IsValid(const FGuid& Guid)
{
	return ToString(Guid) != TEXT("CCCCCCCC-CCCC-CCCC-CCCC-CCCCCCCCCCCC"); // hacky, but it works
}

FString ToString(const FGuid& Guid)
{
	OLECHAR* GuidString;
	StringFromCLSID(Guid, &GuidString); // TODO: Error handling
	const FString String = GuidString;
	CoTaskMemFree(GuidString);
	return String.substr(1, String.size() - 2); // remove curly braces
}
