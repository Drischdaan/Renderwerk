#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Types/String.h"

#include <format>
#include <string>

using FGuid = GUID;

[[nodiscard]] ENGINE_API FGuid NewGuid();
[[nodiscard]] ENGINE_API FGuid NewGuid(FString String);
[[nodiscard]] ENGINE_API bool8 IsValid(const FGuid& Guid);
[[nodiscard]] ENGINE_API FString ToString(const FGuid& Guid);

template <>
struct std::hash<FGuid>
{
	size64 operator()(const FGuid& Guid) const noexcept
	{
		size64 Hash = std::hash<DWORD>()(Guid.Data1);
		Hash ^= std::hash<WORD>()(Guid.Data2);
		Hash ^= std::hash<WORD>()(Guid.Data3);
		for (int i = 0; i < 8; ++i)
		{
			Hash ^= std::hash<BYTE>()(Guid.Data4[i]);
		}
		return Hash;
	}
};
