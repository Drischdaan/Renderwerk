#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/String.hpp"
#include "Renderwerk/Core/Misc/SourceContext.hpp"

#define MAKE_ERROR_ID(Code) ((FErrorId) (((unsigned long)(1)<<31) | ((unsigned long)(4)<<16) | ((unsigned long)(Code))) )

#define ERROR_ID_REPORT MAKE_ERROR_ID(1)

using FErrorId = long;

struct FErrorDetails
{
	FErrorId Id;
	FString Description;
	FSourceContext Context;

	constexpr FErrorDetails(const FErrorId Id, FString Description, const FSourceContext& Context)
		: Id(Id), Description(std::move(Description)), Context(Context)
	{
	}

	constexpr FErrorDetails(const FErrorId Id, const FSourceContext& Context)
		: Id(Id), Context(Context)
	{
	}
};

class ENGINE_API FErrorHandler
{
public:
	static void Report(const FErrorDetails& Details);

	static long Handle(EXCEPTION_POINTERS* Pointers);

public:
	[[nodiscard]] static int32 GetExitCode();

private:
	static int32 ExitCode;
};
