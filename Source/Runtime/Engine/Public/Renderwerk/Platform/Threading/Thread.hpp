#pragma once

#include <atomic>
#include <functional>
#include <Windows.h>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

using FThreadFunc = std::function<int32(void*)>;

enum class ENGINE_API EThreadState : uint8
{
	Created = 0,
	Running,
	Stopped,
};

class ENGINE_API FThread
{
public:
	FThread() = default;
	FThread(FThreadFunc&& InFunction, void* InUserData = nullptr);

	template <typename TOwner>
	FThread(TOwner* Owner, int32 (TOwner::*InFunction)(void*), void* InUserData = nullptr)
		: FThread([Owner, InFunction](void* ThreadUserData) -> int32 { return (Owner->*InFunction)(ThreadUserData); }, InUserData)
	{
	}

	template <typename TOwner>
	FThread(TOwner* Owner, int32 (TOwner::*InFunction)(void*) const, void* InUserData = nullptr)
		: FThread([Owner, InFunction](void* ThreadUserData) -> int32 { return (Owner->*InFunction)(ThreadUserData); }, InUserData)
	{
	}

	~FThread();

	DEFAULT_COPY_MOVEABLE(FThread)

public:
	[[nodiscard]] bool8 IsValid() const;

	void Start();
	void Join(uint64 WaitTime = INFINITE);
	void Kill();

public:
	[[nodiscard]] EThreadState GetState() const { return State; }
	[[nodiscard]] uint64 GetId() const { return Id; }

public:
	[[nodiscard]] static uint64 GetCurrentId();

private:
	FThreadFunc Function = nullptr;
	void* UserData = nullptr;

	std::atomic<EThreadState> State = EThreadState::Created;

	HANDLE ThreadHandle = nullptr;
	uint64 Id = 0;

	friend DWORD ThreadFunc(LPVOID ThreadParameter);
};
