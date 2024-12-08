#include "pch.h"

#include "Renderwerk/Launch/Launch.h"

#include "Renderwerk/Launch/FlowControl.h"

void GuardedMain(const FLaunchParameters& Parameters)
{
	TVector<FString> Arguments;
	for (uint32 Index = 1; Index < Parameters.ArgumentCount; ++Index) // Skip the first argument, which is the executable path
		Arguments.push_back(Parameters.Arguments[Index]);
	GEngine = MakeShared<FEngine>(std::move(Arguments));
	GEngine->Run();
	GEngine.reset();
}

int32 LaunchEngine(const FLaunchParameters& Parameters)
{
	__try
	{
		GuardedMain(Parameters);
	}
	__except (ExceptionHandler(GetExceptionInformation()))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
