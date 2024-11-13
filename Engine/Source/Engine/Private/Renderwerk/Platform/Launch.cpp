#include "pch.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Platform/Logging/LogManager.h"

void StartCoreServices()
{
	FLogManager::Initialize();
}

void ShutdownCoreServices()
{
	FLogManager::Shutdown();
}

int32 LaunchEngine([[maybe_unused]] const FLaunchParameters& Parameters)
{
	StartCoreServices();

	ShutdownCoreServices();
	return EXIT_SUCCESS;
}
