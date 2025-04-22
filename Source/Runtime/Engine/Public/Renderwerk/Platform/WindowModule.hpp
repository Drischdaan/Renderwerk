#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Core/Misc/Guid.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"

struct FWindowDesc;
class FWindow;
class FMonitor;

class ENGINE_API FWindowModule : public IEngineModule
{
public:
	FWindowModule();
	~FWindowModule() override = default;

	DEFAULT_COPY_MOVEABLE(FWindowModule)

public:
	[[nodiscard]] TRef<FWindow> NewWindow(const FWindowDesc& Description);
	void DestroyWindow(const FGuid& Guid);
	void DestroyWindow(TRef<FWindow>&& Window);

	[[nodiscard]] TRef<FWindow> GetWindow(const FGuid& Guid);

private:
	void Initialize() override;
	void Shutdown() override;

	void OnTick();

public:
	DEFINE_MODULE_NAME("Window")

private:
	WNDCLASSEX WindowClass = {};

	FDelegateHandle TickDelegateHandle;

	TVector<TRef<FWindow>> Windows;

	friend LRESULT WindowProcess(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
};
