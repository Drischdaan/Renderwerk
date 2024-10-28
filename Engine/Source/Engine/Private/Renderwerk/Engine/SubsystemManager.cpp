#include "pch.h"

#include "Renderwerk/Engine/SubsystemManager.h"

FSubsystemManager::FSubsystemManager()
{
}

FSubsystemManager::~FSubsystemManager()
{
	for (auto Iterator = Subsystems.rbegin(); Iterator != Subsystems.rend(); ++Iterator)
	{
		TSharedPtr<ISubsystem> Subsystem = Iterator->second;
		Subsystem->Shutdown();
		Subsystem.reset();
	}
	Subsystems.clear();
}
