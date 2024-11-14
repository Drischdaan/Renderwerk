#include "pch.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Platform/Assert.h"
#include "Renderwerk/Platform/Logging/LogManager.h"

void StartCoreServices()
{
	FLogManager::Initialize();
}

void RunEngine()
{
}

void ShutdownCoreServices()
{
	FLogManager::Shutdown();
}

int32 LaunchEngine([[maybe_unused]] const FLaunchParameters& Parameters)
{
	StartCoreServices();
	SetUnhandledExceptionFilter(ExceptionHandler);
	__try
	{
		RunEngine();
	}
	__except (ExceptionHandler(GetExceptionInformation()))
	{
	}
	ShutdownCoreServices();
	return EXIT_SUCCESS;
}
