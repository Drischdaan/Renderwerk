#include "pch.hpp"

#include "Renderwerk/Engine/Engine.hpp"

TRef<FEngine> GEngine = nullptr;

void FEngine::Run()
{
	Initialize();
	RunLoop();
	Shutdown();
}

void FEngine::Initialize()
{
	PrintLogo();
	RW_LOG(Info, "Engine initialized");
}

void FEngine::RunLoop()
{
}

void FEngine::Shutdown()
{
	RW_LOG(Info, "Engine shutdown");
}

void FEngine::PrintLogo()
{
	RW_LOG(Info, R"(    _____                _                             _    )");
	RW_LOG(Info, R"(   |  __ \              | |                           | |   )");
	RW_LOG(Info, R"(   | |__) |___ _ __   __| | ___ _ ____      _____ _ __| | __)");
	RW_LOG(Info, R"(   |  _  // _ \ '_ \ / _` |/ _ \ '__\ \ /\ / / _ \ '__| |/ /)");
	RW_LOG(Info, R"(   | | \ \  __/ | | | (_| |  __/ |   \ V  V /  __/ |  |   < )");
	RW_LOG(Info, R"(   |_|  \_\___|_| |_|\__,_|\___|_|    \_/\_/ \___|_|  |_|\_\)");
	RW_LOG(Info, "");
	RW_LOG(Info, "   {} v{} | Config: {}", TEXT(RW_ENGINE_NAME), TEXT(RW_ENGINE_FULL_VERSION), TEXT(RW_CONFIG));
	RW_LOG(Info, "");
}
