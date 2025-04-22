#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/Misc/Name.hpp"

enum class ENGINE_API EEngineThreadAffinity : uint8
{
	Main = 0,
	Render,
	Update,
};

class ENGINE_API IEngineModule
{
public:
	IEngineModule(const EEngineThreadAffinity InEngineThreadAffinity = EEngineThreadAffinity::Main)
		: EngineThreadAffinity(InEngineThreadAffinity)
	{
	}

	virtual ~IEngineModule() = default;

	DEFAULT_COPY_MOVEABLE(IEngineModule)

public:
	[[nodiscard]] EEngineThreadAffinity GetEngineThreadAffinity() const { return EngineThreadAffinity; }

private:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

	[[nodiscard]] virtual constexpr FName GetName() const = 0;

private:
	EEngineThreadAffinity EngineThreadAffinity;

	friend class ENGINE_API FEngine;
	friend class ENGINE_API FMainThread;
	friend class ENGINE_API FRenderThread;
	friend class ENGINE_API FUpdateThread;
};

#define DEFINE_MODULE_NAME(Name) \
	static constexpr FName StaticModuleName = FStringView(TEXT(Name)); \
	[[nodiscard]] constexpr FName GetName() const override { return StaticModuleName; }
