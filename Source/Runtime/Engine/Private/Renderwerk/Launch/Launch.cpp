#include "pch.hpp"

#include "Renderwerk/Launch/Launch.hpp"

#include "Renderwerk/Core/Error/ErrorHandler.hpp"
#include "Renderwerk/Core/Logging/Logger.hpp"
#include "Renderwerk/Engine/Engine.hpp"

void GuardedMain()
{
	GEngine = NewRef<FEngine>();
	GEngine->Run();
	GEngine.reset();
	RW_VERIFY_MSG(GEngine.use_count() == 0, "Engine is still referenced after shutdown");
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
