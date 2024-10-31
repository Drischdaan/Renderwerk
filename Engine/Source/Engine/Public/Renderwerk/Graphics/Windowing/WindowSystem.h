#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Engine/System.h"
#include "Renderwerk/Graphics/GraphicsFwd.h"

DECLARE_LOG_CATEGORY(LogWindowSystem, Trace);

DECLARE_MULTICAST_DELEGATE(WindowCreated, TSharedPtr<FWindow>);
DECLARE_MULTICAST_DELEGATE(WindowDestroyed, TSharedPtr<FWindow>);

class RENDERWERK_API FWindowSystem : public ISystem
{
public:
	FWindowSystem();
	~FWindowSystem() override;

	DELETE_COPY_AND_MOVE(FWindowSystem);

public:
	void Initialize() override;
	void Shutdown() override;

	NODISCARD bool8 IsRegistered(const FGuid& InGuid) const;
	NODISCARD TSharedPtr<FWindow> Create(const FWindowDesc& InDescription);
	NODISCARD TSharedPtr<FWindow> Get(const FGuid& InGuid) const;
	NODISCARD TVector<TSharedPtr<FWindow>> GetAll() const;
	void Destroy(const FGuid& InGuid);

public:
	NODISCARD FWindowCreatedDelegate& GetWindowCreatedDelegate() { return WindowCreatedDelegate; }
	NODISCARD FWindowDestroyedDelegate& GetWindowDestroyedDelegate() { return WindowDestroyedDelegate; }

private:
	void OnTick(float64 DeltaTime) const;

private:
	TMap<FGuid, TSharedPtr<FWindow>> WindowRegistry;

	WNDCLASSEX WindowClass = {};
	FDelegateHandle OnTickHandle = {};

	FWindowCreatedDelegate WindowCreatedDelegate;
	FWindowDestroyedDelegate WindowDestroyedDelegate;

	TSharedPtr<FWindow> MainWindow;
};
