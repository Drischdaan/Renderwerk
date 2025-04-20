#pragma once

#include <Windows.h>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Error/Assertion.hpp"
#include "Renderwerk/Core/Memory/Memory.hpp"

template <typename T = uint8>
class FDataBuffer
{
public:
	FDataBuffer()
		: Data(nullptr), Size(0), bIsOwningData(false)
	{
	}

	FDataBuffer(T* InData, const size64 InSize, const bool8 InIsOwningData = false)
		: Data(InData), Size(InSize), bIsOwningData(InIsOwningData)
	{
	}

	FDataBuffer(const FDataBuffer& Other)
		: Data(Other.Data), Size(Other.Size), bIsOwningData(false)
	{
	}

	FDataBuffer(FDataBuffer&& Other) noexcept
		: Data(Other.Data), Size(Other.Size), bIsOwningData(Other.bIsOwningData)
	{
		Other.Data = nullptr;
		Other.Size = 0;
		Other.bIsOwningData = false;
	}

	~FDataBuffer()
	{
		if (bIsOwningData)
		{
			FMemory::Free(Data);
		}
	}

public:
	FDataBuffer& operator=(const FDataBuffer& Other)
	{
		if (&Other == this)
		{
			return *this;
		}
		if (bIsOwningData && Data != nullptr)
		{
			FMemory::Free(Data);
		}
		Data = Other.Data;
		Size = Other.Size;
		bIsOwningData = false;
		return *this;
	}

	FDataBuffer& operator=(FDataBuffer&& Other) noexcept
	{
		if (&Other == this)
		{
			return *this;
		}
		if (bIsOwningData && Data != nullptr)
		{
			FMemory::Free(Data);
		}
		Data = Other.Data;
		Size = Other.Size;
		bIsOwningData = Other.bIsOwningData;
		Other.Data = nullptr;
		Other.Size = 0;
		Other.bIsOwningData = false;
		return *this;
	}

public:
	[[nodiscard]] bool8 IsValid() const
	{
		return Data != nullptr;
	}

	[[nodiscard]] bool8 IsEmpty() const
	{
		return Size == 0;
	}

	void Fill(T Value = 0) const
	{
		if (!IsValid())
		{
			return;
		}
		FMemory::Set(Data, Value, Size);
	}

	void Resize(const size64 NewSize)
	{
		void* NewData = FMemory::Allocate(NewSize);
		FMemory::Copy(NewData, Data, std::min(Size, NewSize));
		if (bIsOwningData)
		{
			FMemory::Free(Data);
		}
		Data = static_cast<T*>(NewData);
		Size = NewSize;
	}

	[[nodiscard]] FDataBuffer CreateView(size64 Offset = 0, size64 MaxSize = 0) const
	{
		RW_VERIFY_MSG(Offset + MaxSize <= Size, "Given offset and max size exceeds data buffer size");
		return {Data + Offset, MaxSize == 0 ? (Size - Offset) : MaxSize};
	}

	[[nodiscard]] FDataBuffer CreateCopy(size64 Offset = 0, const size64 MaxSize = 0) const
	{
		RW_VERIFY_MSG(Offset + MaxSize <= Size, "Given offset and max size exceeds data buffer size");
		uint8* NewData = static_cast<uint8*>(FMemory::Allocate(MaxSize));
		size64 CopySize = MaxSize == 0 ? (Size - Offset) : MaxSize;
		FMemory::Copy(NewData, Data + Offset, CopySize);
		return {NewData, CopySize, true};
	}

	[[nodiscard]] FString AsString() const
	{
		if (!IsValid())
		{
			return TEXT("");
		}
		FString Result;
		const int32 RequiredSize = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(Data), static_cast<int>(Size), nullptr, 0);
		if (RequiredSize > 0)
		{
			Result.resize(RequiredSize);
			MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(Data), static_cast<int>(Size), Result.data(), RequiredSize);
		}
		return Result;
	}

public:
	[[nodiscard]] T* GetData() const { return Data; }
	[[nodiscard]] size64 GetSize() const { return Size; }

	[[nodiscard]] bool8 IsOwning() const { return bIsOwningData; }

private:
	T* Data;
	size64 Size;
	bool8 bIsOwningData;
};
