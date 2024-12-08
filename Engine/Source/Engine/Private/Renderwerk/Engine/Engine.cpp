#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

TSharedPtr<FEngine> GEngine = nullptr; // NOLINT(misc-use-internal-linkage)

FEngine::FEngine(TVector<FString>&& InArguments) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
	ArgumentParser = FArgumentParser(std::move(InArguments));
	if (const FArgument& Argument = ArgumentParser.GetArgument(FName(TEXT("test"))))
		printf("Test Argument: %ls\n", Argument.Value.c_str());
	// TODO: Feed into cvar system
}

FEngine::~FEngine()
{
}

void FEngine::Run()
{
	Initialize();
	RunLoop();
	Shutdown();
}

void FEngine::Initialize()
{
	bIsRunning = false;
}

void FEngine::RunLoop()
{
	while (bIsRunning)
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 1)
			RequestShutdown();
	}
}

void FEngine::Shutdown()
{
}

void FEngine::RequestShutdown()
{
	bIsRunning = false;
}

TSharedPtr<FEngine> GetEngine()
{
	VERIFY(GEngine != nullptr, "trying to access invalid engine instance pointer");
	return GEngine;
}
