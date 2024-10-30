#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/Memory/Memory.h"
#include "Renderwerk/Core/Types/CoreTypes.h"

/**
 * @brief Standard library compatible allocator. This allocator is used to allocate memory for STL containers.
 * @tparam T Type of the elements to allocate.
 */
template <typename T>
class RENDERWERK_API TSTLAllocator
{
public:
	using value_type = T;
	using size_type = size64;
	using difference_type = std::ptrdiff_t;

public:
	CONSTEXPR TSTLAllocator() noexcept = default;
	CONSTEXPR TSTLAllocator(const TSTLAllocator&) noexcept = default;
	CONSTEXPR TSTLAllocator(TSTLAllocator&&) noexcept = default;

	template <typename TOther>
	CONSTEXPR TSTLAllocator(const TSTLAllocator<TOther>&) noexcept
	{
	}

	template <typename TOther>
	CONSTEXPR TSTLAllocator(const TSTLAllocator<TOther>&&) noexcept
	{
	}

	CONSTEXPR ~TSTLAllocator() = default;

	CONSTEXPR TSTLAllocator& operator=(const TSTLAllocator&) = default;
	CONSTEXPR TSTLAllocator& operator=(TSTLAllocator&&) = default;

	NODISCARD static CONSTEXPR value_type* allocate(const size_type Count)
	{
		return static_cast<T*>(FMemory::Allocate(Count * sizeof(T)));
	}

	static CONSTEXPR void deallocate(value_type* Pointer, size_type Count) noexcept
	{
		FMemory::Free(Pointer);
	}

	template <typename TOther>
	struct rebind
	{
		using other = TSTLAllocator<TOther>;
	};
};

template <class T, class TOther>
bool operator==(const TSTLAllocator<T>&, const TSTLAllocator<TOther>&) { return true; }

template <class T, class TOther>
bool operator!=(const TSTLAllocator<T>&, const TSTLAllocator<TOther>&) { return false; }
