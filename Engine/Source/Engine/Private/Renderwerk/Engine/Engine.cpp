#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <csignal>

#include "Renderwerk/Engine/SystemManager.h"

DEFINE_LOG_CATEGORY(LogEngine);

TSharedPtr<FEngine> GEngine = nullptr;
FSignalReceivedDelegate FEngine::OnSignalReceived;

FEngine::FEngine()
{
}

FEngine::~FEngine()
{
}

void FEngine::RequestExit()
{
	FScopedLock Lock(RunningMutex);
	bIsRunning = false;
	RW_LOG(LogEngine, Warn, "Exit was requested");
}

void FEngine::Run()
{
	Initialize();

	while (bIsRunning)
	{
		OnTick.Execute(0.0f);
		if (GetAsyncKeyState(VK_ESCAPE) & 1)
			RequestExit();
	}

	Shutdown();
}

void FEngine::Initialize()
{
	RegisterInterruptSignals();
	OnSignalReceived.Bind(BIND_MEMBER_ONE(FEngine::SignalHandler));

	RW_LOG(LogEngine, Info, "Engine initialized");
}

void FEngine::Shutdown()
{
	FSystemManger::Clear();
}

void FEngine::SignalHandler(int32 Signal)
{
	switch (Signal)
	{
	case SIGTERM:
		RW_LOG(LogEngine, Warn, "Termination signal received");
		break;
	case SIGSEGV:
		RW_LOG(LogEngine, Warn, "Segmentation fault signal received");
		break;
	case SIGINT:
		RW_LOG(LogEngine, Warn, "External Interrupt signal received");
		break;
	case SIGILL:
		RW_LOG(LogEngine, Warn, "Illegal Instruction signal received");
		break;
	case SIGABRT:
		RW_LOG(LogEngine, Warn, "Abort signal received");
		break;
	case SIGFPE:
		RW_LOG(LogEngine, Warn, "Erroneous arithmetic operation signal received");
		break;
	default:
		RW_LOG(LogEngine, Warn, "Signal {} received", Signal);
		break;
	}
	RequestExit();
}

void FEngine::RegisterInterruptSignals()
{
	_crt_signal_t SignalHandlerFunc = [](const int Signal)
	{
		if (OnSignalReceived.IsBound())
			OnSignalReceived.Execute(Signal);
	};
	signal(SIGTERM, SignalHandlerFunc);
	signal(SIGSEGV, SignalHandlerFunc);
	signal(SIGINT, SignalHandlerFunc);
	signal(SIGILL, SignalHandlerFunc);
	signal(SIGABRT, SignalHandlerFunc);
	signal(SIGFPE, SignalHandlerFunc);
	RW_LOG(LogEngine, Info, "Signal handlers registered");
}

TSharedPtr<FEngine> GetEngine()
{
	DEBUG_ASSERTM(GEngine != nullptr, "Global engine pointer is invalid.");
	return GEngine;
}
