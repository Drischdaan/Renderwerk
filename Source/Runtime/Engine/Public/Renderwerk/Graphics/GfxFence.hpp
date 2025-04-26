#pragma once

#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Graphics/GfxCommon.hpp"

class ENGINE_API FGfxFence : public IGfxDeviceChild
{
public:
	explicit FGfxFence(FGfxDevice* InGfxDevice);
	FGfxFence(FGfxDevice* InGfxDevice, const FStringView& InDebugName);
	~FGfxFence() override;

	NON_COPY_MOVEABLE(FGfxFence)

public:
	void Signal(uint64 Value);
	void Signal();

	void SignalCommandQueue(const TComPtr<ID3D12CommandQueue>& CommandQueue);

	bool8 Wait(uint64 Value, uint64 WaitTime = INFINITE) const;
	bool8 Wait(uint64 WaitTime = INFINITE) const;

	[[nodiscard]] uint64 GetNextValue() const;

public:
	[[nodiscard]] uint64 GetLastSignaledValue() const { return LastSignaledValue; }

	[[nodiscard]] FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId) override;

private:
	TComPtr<ID3D12Fence1> Fence;
	HANDLE Event;

	uint64 LastSignaledValue = 0;
};
