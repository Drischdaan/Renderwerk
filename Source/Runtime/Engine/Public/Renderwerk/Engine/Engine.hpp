#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"

class ENGINE_API FEngine
{
public:
	FEngine() = default;
	~FEngine() = default;

	DEFAULT_COPY_MOVEABLE(FEngine)

public:

private:
	void Run();

	void Initialize();
	void RunLoop();
	void Shutdown();

private:
	static void PrintLogo();

private:
	friend void GuardedMain();
};

ENGINE_API extern TRef<FEngine> GEngine;

[[nodiscard]] ENGINE_API TRef<FEngine> GetEngine();
