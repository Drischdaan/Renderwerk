#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Threading/ThreadTypes.h"

class FWindow;
struct FWindowDesc;

class ENGINE_API FWindowManager
{
public:
	FWindowManager();
	~FWindowManager();

	DELETE_COPY_AND_MOVE(FWindowManager);

public:
	[[nodiscard]] FGuid Create(const FWindowDesc& Description);
	[[nodiscard]] TSharedPtr<FWindow> CreateAndGet(const FWindowDesc& Description);

	[[nodiscard]] bool8 IsRegistered(const FGuid& Id) const;
	[[nodiscard]] TSharedPtr<FWindow> Get(const FGuid& Id);

	void Destroy(const FGuid& Id);
	void Destroy(const TSharedPtr<FWindow>& Window);

	void ProcessMessages();
	void ProcessInvalidWindows();

public:
	[[nodiscard]] WNDCLASSEX GetWindowClass() const { return WindowClass; }

private:
	FMutex Mutex;
	TMap<FGuid, TSharedPtr<FWindow>> Windows;
	WNDCLASSEX WindowClass;

	TQueue<FGuid> InvalidWindowQueue;
};
