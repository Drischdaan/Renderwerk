#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Memory/Memory.hpp"
#include "Renderwerk/Core/Misc/Atomic.hpp"

#include <dxgi1_6.h>
#include <dxgidebug.h>
#include "directx/d3d12.h"
#include "directx/d3d12sdklayers.h"
#include "directx/d3dx12.h"

class ENGINE_API IGraphicsObject
{
public:
	IGraphicsObject() = default;
	virtual ~IGraphicsObject() = default;

	NON_COPY_MOVEABLE(IGraphicsObject)

public:
	virtual uint64 AddRef() = 0;
	virtual uint64 Release() = 0;
};

template <typename T>
class ENGINE_API TGraphicsObject : public IGraphicsObject
{
public:
	uint64 AddRef() override
	{
		return ++RefCount;
	}

	uint64 Release() override
	{
		const uint64 TempRefCount = --RefCount;
		if (TempRefCount == 0)
		{
			FMemory::DestroyInstance(this);
		}
		return TempRefCount;
	}

	[[nodiscard]] uint64 GetRefCount() const
	{
		return RefCount.load();
	}

protected:
	TAtomic<uint64> RefCount = 0;
};

template <typename T>
class ENGINE_API TObjectHandle
{
public:
	TObjectHandle()
		: Pointer(nullptr)
	{
	}

	TObjectHandle(std::nullptr_t)
		: Pointer(nullptr)
	{
	}

	TObjectHandle(const TObjectHandle& Other)
		: Pointer(Other.Pointer)
	{
		InternalAddRef();
	}

	TObjectHandle(T* OtherPointer)
		: Pointer(OtherPointer)
	{
		InternalAddRef();
	}

	template <class TOther>
	TObjectHandle(TOther* OtherPointer)
		: Pointer(OtherPointer)
	{
		InternalAddRef();
	}

	TObjectHandle(TObjectHandle&& Other) noexcept
		: Pointer(nullptr)
	{
		Swap(Other);
	}

	template <typename TOther> requires std::is_convertible_v<T*, TOther*>
	TObjectHandle(const TObjectHandle<TOther>& Other)
		: Pointer(Other.Pointer)
	{
		InternalAddRef();
	}

	template <typename TOther> requires std::is_convertible_v<T*, TOther*>
	TObjectHandle(TObjectHandle<TOther>&& Other) noexcept
		: Pointer(Other.Pointer)
	{
		std::move(Other).Pointer = nullptr;
	}

	~TObjectHandle()
	{
		InternalRelease();
	}

public:
	TObjectHandle& operator=(std::nullptr_t)
	{
		InternalRelease();
		return *this;
	}

	TObjectHandle& operator=(T* OtherPointer)
	{
		if (Pointer != OtherPointer)
		{
			TObjectHandle(OtherPointer).Swap(*this);
		}
		return *this;
	}

	template <typename TOther>
	TObjectHandle& operator=(TOther* OtherPointer)
	{
		TObjectHandle(OtherPointer).Swap(*this);
		return *this;
	}

	TObjectHandle& operator=(const TObjectHandle& Other)
	{
		if (Pointer != Other.Pointer)
		{
			TObjectHandle(Other).Swap(*this);
		}
		return *this;
	}

	template <class TOther>
	TObjectHandle& operator=(const TObjectHandle<TOther>& Other) noexcept
	{
		TObjectHandle(Other).Swap(*this);
		return *this;
	}

	TObjectHandle& operator=(TObjectHandle&& Other) noexcept
	{
		TObjectHandle(static_cast<TObjectHandle&&>(std::move(Other))).Swap(*this);
		return *this;
	}

	template <class TOther>
	TObjectHandle& operator=(TObjectHandle<TOther>&& Other) noexcept
	{
		TObjectHandle(static_cast<TObjectHandle<TOther>&&>(std::move(Other))).Swap(*this);
		return *this;
	}

public:
	void Swap(TObjectHandle& Other) noexcept
	{
		T* TempPointer = Pointer;
		Pointer = Other.Pointer;
		Other.Pointer = TempPointer;
	}

	void Swap(TObjectHandle&& Other) noexcept
	{
		T* TempPointer = Pointer;
		Pointer = Other.Pointer;
		std::move(Other).Pointer = TempPointer;
	}

public:
	[[nodiscard]] T* Get() const
	{
		return Pointer;
	}

	[[nodiscard]] T** GetAddressOf() noexcept
	{
		return &Pointer;
	}

	[[nodiscard]] T* const* GetAddressOf() const
	{
		return &Pointer;
	}

	void Attach(T* OtherPointer)
	{
		if (Pointer != nullptr)
		{
			const uint64 ReferenceCount = Pointer->Release();
			RW_VERIFY_MSG(ReferenceCount != 0 && Pointer != OtherPointer, "Attaching to the same object is not allowed");
		}
		Pointer = OtherPointer;
	}

	[[nodiscard]] T* Detach() noexcept
	{
		T* TempPointer = Pointer;
		Pointer = nullptr;
		return TempPointer;
	}

	uint64 Reset()
	{
		return InternalRelease();
	}

public:
	operator T*() const
	{
		return Pointer;
	}

	T* operator->() const
	{
		return Pointer;
	}

	T** operator&() // NOLINT(google-runtime-operator)
	{
		return &Pointer;
	}

private:
	void InternalAddRef()
	{
		if (Pointer != nullptr)
		{
			Pointer->AddRef();
		}
	}

	uint64 InternalRelease()
	{
		uint64 ReferenceCount = 0;
		T* TempPointer = Pointer;
		if (TempPointer != nullptr)
		{
			Pointer = nullptr;
			ReferenceCount = TempPointer->Release();
		}
		return ReferenceCount;
	}

private:
	T* Pointer;

	template <typename TOther>
	friend class TObjectHandle;
};

template <typename TObject, typename... TArguments> requires std::is_constructible_v<TObject, TArguments...>
[[nodiscard]] TObjectHandle<TObject> NewObjectHandle(TArguments&&... Arguments)
{
	return TObjectHandle<TObject>(FMemory::NewInstance<TObject>(std::forward<TArguments>(Arguments)...));
}

_EXPORT_STD template <typename T, typename TOther>
_NODISCARD TObjectHandle<T> CastObjectHandle(const TObjectHandle<TOther>& Other) noexcept
{
	const T* Pointer = static_cast<T*>(Other.Get());
	return TObjectHandle<T>(Pointer);
}

namespace D3DUtility
{
	[[nodiscard]] constexpr FStringView ToString(const D3D_FEATURE_LEVEL FeatureLevel)
	{
		switch (FeatureLevel)
		{
		case D3D_FEATURE_LEVEL_1_0_GENERIC: return TEXT("D3D_FEATURE_LEVEL_1_0_GENERIC");
		case D3D_FEATURE_LEVEL_1_0_CORE: return TEXT("D3D_FEATURE_LEVEL_1_0_CORE");
		case D3D_FEATURE_LEVEL_9_1: return TEXT("D3D_FEATURE_LEVEL_9_1");
		case D3D_FEATURE_LEVEL_9_2: return TEXT("D3D_FEATURE_LEVEL_9_2");
		case D3D_FEATURE_LEVEL_9_3: return TEXT("D3D_FEATURE_LEVEL_9_3");
		case D3D_FEATURE_LEVEL_10_0: return TEXT("D3D_FEATURE_LEVEL_10_0");
		case D3D_FEATURE_LEVEL_10_1: return TEXT("D3D_FEATURE_LEVEL_10_1");
		case D3D_FEATURE_LEVEL_11_0: return TEXT("D3D_FEATURE_LEVEL_11_0");
		case D3D_FEATURE_LEVEL_11_1: return TEXT("D3D_FEATURE_LEVEL_11_1");
		case D3D_FEATURE_LEVEL_12_0: return TEXT("D3D_FEATURE_LEVEL_12_0");
		case D3D_FEATURE_LEVEL_12_1: return TEXT("D3D_FEATURE_LEVEL_12_1");
		case D3D_FEATURE_LEVEL_12_2: return TEXT("D3D_FEATURE_LEVEL_12_2");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D_SHADER_MODEL ShaderModel)
	{
		switch (ShaderModel)
		{
		case D3D_SHADER_MODEL_NONE: return TEXT("D3D_SHADER_MODEL_NONE");
		case D3D_SHADER_MODEL_5_1: return TEXT("D3D_SHADER_MODEL_5_1");
		case D3D_SHADER_MODEL_6_0: return TEXT("D3D_SHADER_MODEL_6_0");
		case D3D_SHADER_MODEL_6_1: return TEXT("D3D_SHADER_MODEL_6_1");
		case D3D_SHADER_MODEL_6_2: return TEXT("D3D_SHADER_MODEL_6_2");
		case D3D_SHADER_MODEL_6_3: return TEXT("D3D_SHADER_MODEL_6_3");
		case D3D_SHADER_MODEL_6_4: return TEXT("D3D_SHADER_MODEL_6_4");
		case D3D_SHADER_MODEL_6_5: return TEXT("D3D_SHADER_MODEL_6_5");
		case D3D_SHADER_MODEL_6_6: return TEXT("D3D_SHADER_MODEL_6_6");
		case D3D_SHADER_MODEL_6_7: return TEXT("D3D_SHADER_MODEL_6_7");
		case D3D_SHADER_MODEL_6_8: return TEXT("D3D_SHADER_MODEL_6_8");
		case D3D_SHADER_MODEL_6_9: return TEXT("D3D_SHADER_MODEL_6_9");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_RESOURCE_BINDING_TIER ResourceBindingTier)
	{
		switch (ResourceBindingTier)
		{
		case D3D12_RESOURCE_BINDING_TIER_1: return TEXT("D3D12_RESOURCE_BINDING_TIER_1");
		case D3D12_RESOURCE_BINDING_TIER_2: return TEXT("D3D12_RESOURCE_BINDING_TIER_2");
		case D3D12_RESOURCE_BINDING_TIER_3: return TEXT("D3D12_RESOURCE_BINDING_TIER_3");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_RAYTRACING_TIER RaytracingTier)
	{
		switch (RaytracingTier)
		{
		case D3D12_RAYTRACING_TIER_NOT_SUPPORTED: return TEXT("D3D12_RAYTRACING_TIER_NOT_SUPPORTED");
		case D3D12_RAYTRACING_TIER_1_0: return TEXT("D3D12_RAYTRACING_TIER_1_0");
		case D3D12_RAYTRACING_TIER_1_1: return TEXT("D3D12_RAYTRACING_TIER_1_1");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_MESH_SHADER_TIER MeshShaderTier)
	{
		switch (MeshShaderTier)
		{
		case D3D12_MESH_SHADER_TIER_NOT_SUPPORTED: return TEXT("D3D12_MESH_SHADER_TIER_NOT_SUPPORTED");
		case D3D12_MESH_SHADER_TIER_1: return TEXT("D3D12_MESH_SHADER_TIER_1");
		}
		return TEXT("Unknown");
	}

	[[nodiscard]] constexpr FStringView ToString(const D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType)
	{
		switch (DescriptorHeapType)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV");
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER");
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_RTV");
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_DSV");
		case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES: return TEXT("D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES");
		}
		return TEXT("Unknown");
	}
}
