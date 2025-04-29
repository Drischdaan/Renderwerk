#include "pch.hpp"

#include "Renderwerk/Graphics/GfxFence.hpp"

#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FGfxFence::FGfxFence(FGfxDevice* InGfxDevice)
	: FGfxFence(InGfxDevice, TEXT("UnnamedFence"))
{
}

FGfxFence::FGfxFence(FGfxDevice* InGfxDevice, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName)
{
	ID3D12Device14* NativeDevice = GfxDevice->GetNativeObject<ID3D12Device14>(NativeObjectIds::D3D12_Device);
	const HRESULT Result = NativeDevice->CreateFence(LastSignaledValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	RW_VERIFY_ID(Result);

	Event = CreateEvent(nullptr, false, false, nullptr);
}

FGfxFence::~FGfxFence()
{
	CloseHandle(Event);
	Fence.Reset();
}

void FGfxFence::Signal(const uint64 Value)
{
	const HRESULT Result = Fence->Signal(Value);
	RW_VERIFY_ID(Result);
	LastSignaledValue = Value;
}

void FGfxFence::Signal()
{
	Signal(GetNextValue());
}

void FGfxFence::SignalCommandQueue(const TComPtr<ID3D12CommandQueue>& CommandQueue)
{
	const uint64 SignalValue = GetNextValue();
	const HRESULT Result = CommandQueue->Signal(Fence.Get(), SignalValue);
	RW_VERIFY_ID(Result);
	LastSignaledValue = SignalValue;
}

bool8 FGfxFence::Wait(const uint64 Value, const uint64 WaitTime) const
{
	PROFILE_FUNCTION();
	const HRESULT Result = Fence->SetEventOnCompletion(Value, Event);
	RW_VERIFY_ID(Result);
	return WaitForSingleObject(Event, static_cast<DWORD>(WaitTime)) == WAIT_OBJECT_0;
}

bool8 FGfxFence::Wait(const uint64 WaitTime) const
{
	return Wait(LastSignaledValue, WaitTime);
}

uint64 FGfxFence::GetNextValue() const
{
	return LastSignaledValue + 1;
}

FNativeObject FGfxFence::GetRawNativeObject(const FNativeObjectId NativeObjectId)
{
	if (NativeObjectId == NativeObjectIds::D3D12_Fence)
	{
		return Fence.Get();
	}
	return IGfxDeviceChild::GetRawNativeObject(NativeObjectId);
}
