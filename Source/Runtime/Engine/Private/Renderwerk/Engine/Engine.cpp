#include "pch.hpp"

#include "Renderwerk/Engine/Engine.hpp"

#include "Renderwerk/Core/Misc/Guid.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"
#include "Renderwerk/Platform/WindowModule.hpp"

TRef<FEngine> GEngine = nullptr;

void FEngine::RequestShutdown()
{
	RW_LOG(Warning, "Shutdown was requested");
	bShouldRun = false;
}

bool8 FEngine::IsModuleRegistered(const FName Name) const
{
	return Modules.contains(Name);
}

TRef<IEngineModule> FEngine::GetModule(const FName Name)
{
	RW_VERIFY_MSG(IsModuleRegistered(Name), "Module is not registered");
	return Modules.at(Name);
}

void FEngine::RegisterModule(FName Name, const TRef<IEngineModule>& Module)
{
	Modules.insert(std::make_pair(Name, Module));
}

void FEngine::UnregisterModule(const FName Name)
{
	{
		const TRef<IEngineModule> Module = Modules.at(Name);
		Module->Shutdown();
	}
	Modules.erase(Name);
}

void FEngine::Run()
{
	PrintLogo();

	RegisterModule<FWindowModule>();

	MainThread = NewOwned<FMainThread>(&bShouldRun);
	RenderThread = NewOwned<FRenderThread>(&bShouldRun);
	UpdateThread = NewOwned<FUpdateThread>(&bShouldRun);

	MainThread->Initialize();
	while (bShouldRun)
	{
		MainThread->OnTick();
	}
	MainThread->Shutdown();

	MainThread.reset();
	RenderThread.reset();
	UpdateThread.reset();

	Modules.clear();
}

TVector<TRef<IEngineModule>> FEngine::GetModuleList()
{
	TVector<TRef<IEngineModule>> ModuleList;
	ModuleList.reserve(Modules.size());
	for (TRef<IEngineModule>& Module : Modules | std::views::values)
	{
		ModuleList.push_back(Module);
	}
	return ModuleList;
}

TVector<TRef<IEngineModule>> FEngine::GetModuleListByAffinity(const EEngineThreadAffinity Affinity)
{
	TVector<TRef<IEngineModule>> ModuleList;
	ModuleList.reserve(Modules.size());
	for (TRef<IEngineModule>& Module : Modules | std::views::values)
	{
		if (Module->GetEngineThreadAffinity() == Affinity)
		{
			ModuleList.push_back(Module);
		}
	}
	return ModuleList;
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

TRef<FEngine> GetEngine()
{
	RW_VERIFY_MSG(GEngine.get() != nullptr, "Invalid engine instance");
	return GEngine;
}
