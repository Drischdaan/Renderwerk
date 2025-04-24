#include "pch.hpp"

#include "Renderwerk/Renderer/Fence.hpp"

#include "Renderwerk/Profiler/Profiler.hpp"
#include "Renderwerk/Renderer/Device.hpp"

FFence::FFence(FDevice* InDevice)
	: Device(InDevice)
{
	const HRESULT Result = Device->GetHandle()->CreateFence(LastSignaledValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	RW_VERIFY_ID(Result);

	Event = CreateEvent(nullptr, false, false, nullptr);
}

FFence::~FFence()
{
	CloseHandle(Event);
	Fence.Reset();
}

bool8 FFence::IsValueCompleted(const uint64 Value) const
{
	return Fence->GetCreationFlags() >= Value;
}

bool8 FFence::IsValueCompleted() const
{
	return IsValueCompleted(LastSignaledValue);
}

void FFence::Signal(const uint64 Value)
{
	const HRESULT Result = Fence->Signal(Value);
	RW_VERIFY_ID(Result);
	SetSignaledValue(Value);
}

void FFence::Signal()
{
	Signal(GetNextSignalValue());
}

bool8 FFence::Wait(const uint64 Value, const uint64 WaitTime) const
{
	if (IsValueCompleted(Value))
	{
		return true;
	}
	const HRESULT Result = Fence->SetEventOnCompletion(Value, Event);
	RW_VERIFY_ID(Result);
	return WaitForSingleObject(Event, static_cast<DWORD>(WaitTime)) == WAIT_OBJECT_0;
}

bool8 FFence::Wait(const uint64 WaitTime) const
{
	PROFILE_FUNCTION();
	return Wait(LastSignaledValue, WaitTime);
}

void FFence::SetSignaledValue(const uint64 Value)
{
	LastSignaledValue = Value;
}

uint64 FFence::GetNextSignalValue() const
{
	return LastSignaledValue + 1;
}
