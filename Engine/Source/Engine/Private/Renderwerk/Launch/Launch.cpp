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
	START_PROFILER();
	FLogManager::Initialize();
	SetUnhandledExceptionFilter(ExceptionHandler);
	__try
	{
		GuardedMain(Parameters);
	}
	__except (ExceptionHandler(GetExceptionInformation()))
	{
		return EXIT_FAILURE;
	}
	FLogManager::Shutdown();
	SHUTDOWN_PROFILER();
	return EXIT_SUCCESS;
}
