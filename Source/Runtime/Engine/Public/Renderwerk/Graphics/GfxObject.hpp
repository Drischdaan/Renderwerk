#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

using FNativeObject = void*;

using FNativeObjectId = uint32;

class ENGINE_API IGfxObject
{
public:
	explicit IGfxObject() = default;

	IGfxObject(const FStringView& InDebugName)
		: DebugName(InDebugName)
	{
	}

	virtual ~IGfxObject() = default;

	NON_COPY_MOVEABLE(IGfxObject)

public:
	virtual void SetDebugName(const FStringView& NewDebugName)
	{
		DebugName = NewDebugName;
	}

	[[nodiscard]] virtual FNativeObject GetRawNativeObject(FNativeObjectId NativeObjectId)
	{
		RW_ASSERT_MSG(false, "Unable to get native object with id '0x{:x}'", NativeObjectId);
		return nullptr;
	}

public:
	template <typename T>
	[[nodiscard]] T* GetNativeObject(const FNativeObjectId NativeObjectId)
	{
		return static_cast<T*>(GetRawNativeObject(NativeObjectId));
	}

public:
	[[nodiscard]] FString GetDebugName() const { return DebugName; }

private:
	FString DebugName = TEXT("Unnamed");
};

#define DEFINE_NATIVE_OBJECT_ID(Category, ObjectName, Id) \
	constexpr FNativeObjectId Category##_##ObjectName = (static_cast<uint32>(ENativeObjectCategory::Category) | Id)

namespace NativeObjectMasks
{
	constexpr uint32 CategoryMask = 0xFFFF0000;
	constexpr uint32 ObjectMask = 0x0000FFFF;
}

enum class ENGINE_API ENativeObjectCategory : uint32
{
	DXGI = 0x00010000,
	D3D12 = 0x00020000,
};

namespace NativeObjectIds
{
	DEFINE_NATIVE_OBJECT_ID(DXGI, Debug, 1);
	DEFINE_NATIVE_OBJECT_ID(DXGI, Factory, 2);
	DEFINE_NATIVE_OBJECT_ID(DXGI, Adapter, 3);
	DEFINE_NATIVE_OBJECT_ID(DXGI, Swapchain, 4);

	DEFINE_NATIVE_OBJECT_ID(D3D12, Debug, 1);
	DEFINE_NATIVE_OBJECT_ID(D3D12, Device, 2);
	DEFINE_NATIVE_OBJECT_ID(D3D12, CommandQueue, 3);
	DEFINE_NATIVE_OBJECT_ID(D3D12, GraphicsCommandQueue, 4);
	DEFINE_NATIVE_OBJECT_ID(D3D12, ComputeCommandQueue, 5);
	DEFINE_NATIVE_OBJECT_ID(D3D12, CopyCommandQueue, 6);
	DEFINE_NATIVE_OBJECT_ID(D3D12, DescriptorHeap, 7);
	DEFINE_NATIVE_OBJECT_ID(D3D12, Resource, 8);
	DEFINE_NATIVE_OBJECT_ID(D3D12, CommandAllocator, 9);
	DEFINE_NATIVE_OBJECT_ID(D3D12, CommandList, 10);
	DEFINE_NATIVE_OBJECT_ID(D3D12, Fence, 11);
}
