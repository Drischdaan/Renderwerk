#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Window.h"

class ENGINE_API FWindowManager
{
private:
	static void Initialize();
	static void Update();
	static void Shutdown();

	static void HandleMessages();
	static void CollectInvalidWindows();
	static void ProcessDestructionQueue();

public:
	[[nodiscard]] static bool8 IsRegistered(const FGuid& Guid);
	[[nodiscard]] static TSharedPtr<FWindow> NewWindow(const FWindowDesc& Description);
	[[nodiscard]] static TSharedPtr<FWindow> GetWindow(const FGuid& Guid);
	static void DestroyWindow(const FGuid& Guid);

public:
	[[nodiscard]] static WNDCLASSEX GetWindowClass() { return WindowClass; }

private:
	static WNDCLASSEX WindowClass;
	static TMap<FGuid, TSharedPtr<FWindow>> WindowRegistry;

	static TQueue<FGuid> DestructionQueue;

	friend class ENGINE_API FEngine;
};
