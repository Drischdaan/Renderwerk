#include "pch.h"

#include "Renderwerk/Launch.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

void InitializeCore()
{
	FLogManager::Initialize();
	FPlatform::Initialize();
}

void GuardedMain()
{
	GEngine = MakeShared<FEngine>();
	GEngine->Run();
}

void ShutdownCore()
{
	FPlatform::Shutdown();
	FLogManager::Shutdown();
}

int32 LaunchRenderwerk()
{
	int32 ExitCode = EXIT_SUCCESS;
	InitializeCore();
	__try
	{
		GuardedMain();
	}
	__except (FExceptionHandling::Handler(GetExceptionInformation()))
	{
		ExitCode = EXIT_FAILURE;
	}
	ShutdownCore();
	return ExitCode;
}
