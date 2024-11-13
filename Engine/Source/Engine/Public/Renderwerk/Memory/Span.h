#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

/**
 * A span is a non-owning reference to a contiguous sequence of objects.
 * It is a view into an existing array, and does not own the data it points to.
 * It is the caller's responsibility to ensure that the data outlives the span.
 */
template <typename T>
struct Span
{
public:
	Span(T* Data, size64 Size);
	Span(TInitializerList<T> List);

public:
	[[nodiscard]] T* begin() const { return Data; }
	[[nodiscard]] T* end() const { return Data + Size; }

	[[nodiscard]] T& operator[](const size64 Index) const { return Data[Index]; }
	[[nodiscard]] const T& operator[](const size64 Index) { return Data[Index]; }

public:
	[[nodiscard]] size64 GetSize() const { return Size; }

private:
	T* Data;
	size64 Size;
};

template <typename T>
Span<T>::Span(T* Data, const size64 Size)
	: Data(Data), Size(Size)
{
}

template <typename T>
Span<T>::Span(TInitializerList<T> List)
	: Data(List.begin()), Size(List.size())
{
}
