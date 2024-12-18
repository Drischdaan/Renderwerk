#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include "Renderwerk/Platform/Filesystem.h"
#include "Renderwerk/Platform/JSON.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Platform/WindowManager.h"

TSharedPtr<FEngine> GEngine = nullptr; // NOLINT(misc-use-internal-linkage)

FEngine::FEngine(TVector<FString>&& InArguments) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
	ArgumentParser = FArgumentParser(std::move(InArguments));
}

FEngine::~FEngine() = default;

void FEngine::Run()
{
	Initialize();
	RunLoop();
	Shutdown();
}

void FEngine::Initialize()
{
	PROFILER_SET_THREAD_NAME("MainThread");

	WindowManager = MakeShared<FWindowManager>();
	const FWindowDesc MainWindowDesc = {};
	MainWindowGuid = WindowManager->Create(MainWindowDesc);

	RenderThread = MakeShared<FRenderThread>(&bIsRunning);
	UpdateThread = MakeShared<FUpdateThread>(&bIsRunning);
}

void FEngine::RunLoop()
{
	while (bIsRunning)
	{
		PROFILE_FRAME();
		WindowManager->ProcessMessages();
		if (!WindowManager->IsRegistered(MainWindowGuid))
		{
			RequestShutdown();
			// ReSharper disable once CppRedundantControlFlowJump
			continue;
		}
	}
}

void FEngine::Shutdown()
{
	UpdateThread->Signal();
	RenderThread->Signal();
	UpdateThread->Destroy();
	UpdateThread.reset();
	RenderThread->Destroy();
	RenderThread.reset();
	WindowManager.reset();
}

void FEngine::RequestShutdown()
{
	bIsRunning = false;
}

TSharedPtr<FEngine> GetEngine() // NOLINT(misc-use-internal-linkage)
{
	VERIFY(GEngine != nullptr, "Trying to access invalid engine instance pointer");
	return GEngine;
}
