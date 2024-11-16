#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/Types/PrimitiveTypes.h"
#include "Renderwerk/Core/Types/StringTypes.h"

#include <format>
#include <string>

class ENGINE_API FGuid
{
public:
	FGuid();
	FGuid(const TVector<uint8>& InData);
	FGuid(const std::string& InDataString);
	~FGuid() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FGuid);

public:
	[[nodiscard]] bool IsValid() const;
	[[nodiscard]] FString ToString() const;

public:
	[[nodiscard]] TVector<uint8> GetData() const { return Data; }

public:
	bool operator==(const FGuid& Other) const;
	bool operator!=(const FGuid& Other) const;
	bool operator<(const FGuid& Other) const;
	bool operator>(const FGuid& Other) const;

	operator FString() const;

private:
	TVector<uint8> Data;
};

ENGINE_API FGuid NewGuid();

ENGINE_API FString ToString(const FGuid& Guid);

template <>
struct std::hash<FGuid>
{
	size_t operator()(const FGuid& Guid) const noexcept
	{
		const uint64* Data = reinterpret_cast<const uint64*>(Guid.GetData().data());
		return std::hash<uint64>{}(Data[0]) ^ std::hash<uint64>{}(Data[1]);
	}
};

template <>
struct std::formatter<FGuid> : std::formatter<FString>
{
	template <typename FormatContext>
	auto format(const FGuid& Guid, FormatContext& Context) const
	{
		return std::formatter<FString>::format(Guid.ToString(), Context);
	}
};
