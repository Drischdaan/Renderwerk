#pragma once

#include "EngineModule.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/Containers/Map.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Core/Misc/Atomic.hpp"
#include "Renderwerk/Core/Misc/Name.hpp"
#include "Renderwerk/Engine/Threads/MainThread.hpp"
#include "Renderwerk/Engine/Threads/RenderThread.hpp"
#include "Renderwerk/Engine/Threads/UpdateThread.hpp"

class IEngineModule;

class ENGINE_API FEngine
{
public:
	FEngine() = default;
	~FEngine() = default;

	DEFAULT_COPY_MOVEABLE(FEngine)

public:
	void RequestShutdown();

	[[nodiscard]] bool8 IsModuleRegistered(FName Name) const;
	[[nodiscard]] TRef<IEngineModule> GetModule(FName Name);

	void RegisterModule(FName Name, const TRef<IEngineModule>& Module);
	void UnregisterModule(FName Name);

public:
	template <typename T> requires std::is_base_of_v<IEngineModule, T>
	[[nodiscard]] bool8 IsModuleRegistered()
	{
		return IsModuleRegistered(T::StaticModuleName);
	}

	template <typename T> requires std::is_base_of_v<IEngineModule, T>
	[[nodiscard]] TRef<T> GetModule()
	{
		return std::static_pointer_cast<T>(GetModule(T::StaticModuleName));
	}

	template <typename T> requires std::is_base_of_v<IEngineModule, T>
	void RegisterModule()
	{
		RegisterModule(T::StaticModuleName, NewRef<T>());
	}

	template <typename T> requires std::is_base_of_v<IEngineModule, T>
	void UnregisterModule()
	{
		UnregisterModule(T::StaticModuleName);
	}

public:
	[[nodiscard]] TMulticastDelegate<>& GetMainThreadTickDelegate() { return OnMainThreadTick; }
	[[nodiscard]] TMulticastDelegate<>& GetRenderThreadTickDelegate() { return OnRenderThreadTick; }
	[[nodiscard]] TMulticastDelegate<>& GetUpdateThreadTickDelegate() { return OnUpdateThreadTick; }

	[[nodiscard]] TRef<FWindow> GetWindow() const { return MainThread->Window; }

private:
	void Run();

	[[nodiscard]] TVector<TRef<IEngineModule>> GetModuleList();
	[[nodiscard]] TVector<TRef<IEngineModule>> GetModuleListByAffinity(EEngineThreadAffinity Affinity);

private:
	static void PrintLogo();

private:
	TAtomic<bool8> bShouldRun = true;

	TMap<FName, TRef<IEngineModule>> Modules;

	TOwned<FMainThread> MainThread;
	TOwned<FRenderThread> RenderThread;
	TOwned<FUpdateThread> UpdateThread;

	TMulticastDelegate<> OnMainThreadTick;
	TMulticastDelegate<> OnRenderThreadTick;
	TMulticastDelegate<> OnUpdateThreadTick;

	friend void GuardedMain();

	friend class ENGINE_API FMainThread;
	friend class ENGINE_API FRenderThread;
	friend class ENGINE_API FUpdateThread;
};

ENGINE_API extern TRef<FEngine> GEngine;

[[nodiscard]] ENGINE_API TRef<FEngine> GetEngine();
