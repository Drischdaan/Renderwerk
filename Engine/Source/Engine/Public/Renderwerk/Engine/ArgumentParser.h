#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

struct ENGINE_API FArgument
{
	FName Name;
	FString Value;

public:
	[[nodiscard]] bool8 IsValid() const;

public:
	operator bool8() const { return IsValid(); }
};

class ENGINE_API FArgumentParser
{
public:
	FArgumentParser() = default;
	FArgumentParser(TVector<FString>&& InArguments);
	~FArgumentParser();

	DEFINE_DEFAULT_COPY_AND_MOVE(FArgumentParser);

public:
	[[nodiscard]] bool8 HasArgument(const FName& Name) const;
	[[nodiscard]] FArgument& GetArgument(const FName& Name);

private:
	TMap<FName, FArgument> ArgumentMap;
};
