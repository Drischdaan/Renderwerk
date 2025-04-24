#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FDevice;

class ENGINE_API FFence : public TGraphicsObject<FFence>
{
public:
	FFence(FDevice* InDevice);
	~FFence() override;

	NON_COPY_MOVEABLE(FFence)

public:
	[[nodiscard]] bool8 IsValueCompleted(uint64 Value) const;
	[[nodiscard]] bool8 IsValueCompleted() const;

	void Signal(uint64 Value);
	void Signal();

	bool8 Wait(uint64 Value, uint64 WaitTime = INFINITE) const;
	bool8 Wait(uint64 WaitTime = INFINITE) const;

	void SetSignaledValue(uint64 Value);
	[[nodiscard]] uint64 GetNextSignalValue() const;

public:
	[[nodiscard]] TObjectHandle<ID3D12Fence1> GetHandle() const { return Fence; }

private:
	FDevice* Device;

	TObjectHandle<ID3D12Fence1> Fence;
	HANDLE Event;

	uint64 LastSignaledValue = 0;

	friend class ENGINE_API FDevice;
};
