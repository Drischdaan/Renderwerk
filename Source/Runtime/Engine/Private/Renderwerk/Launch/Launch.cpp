#include "pch.hpp"

#include "Renderwerk/Launch/Launch.hpp"

#include "Renderwerk/Core/Error/ErrorHandler.hpp"
#include "Renderwerk/Core/Logging/Logger.hpp"

void GuardedMain()
{
}

int32 Launch()
{
	FLogger::Initialize();
	SetUnhandledExceptionFilter(FErrorHandler::Handle);
	__try
	{
		GuardedMain();
	}
	__except (FErrorHandler::Handle(GetExceptionInformation()))
	{
	}
	FLogger::Shutdown();
	return FErrorHandler::GetExitCode();
}
