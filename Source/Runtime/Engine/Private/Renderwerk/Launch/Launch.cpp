#include "pch.hpp"

#include "Renderwerk/Launch/Launch.hpp"

#include "Renderwerk/Core/Error/ErrorHandler.hpp"
#include "Renderwerk/Core/Logging/Logger.hpp"
#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

void GuardedMain()
{
	GEngine = NewRef<FEngine>();
	GEngine->Run();
	GEngine.reset();
	RW_VERIFY_MSG(GEngine.use_count() == 0, "Engine is still referenced after shutdown");
#ifdef TRACY_MANUAL_LIFETIME
	RW_LOG(Warning, "Manual profiler lifetime activated. Program shutdown will stall until data is transferred");
#endif
}

int32 Launch()
{
	FProfiler::Initialize();
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
	FProfiler::Shutdown();
	return FErrorHandler::GetExitCode();
}
