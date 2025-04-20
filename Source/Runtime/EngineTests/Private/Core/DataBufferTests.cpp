#include "catch_amalgamated.hpp"

#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Memory/DataBuffer.hpp"

#include "Renderwerk/Core/Containers/String.hpp"

TEST_CASE("FDataBuffer default constructor", "[DataBuffer]")
{
	const FDataBuffer<uint8> Buffer;

	REQUIRE_FALSE(Buffer.IsValid());
	REQUIRE(Buffer.IsEmpty());
	REQUIRE(Buffer.GetData() == nullptr);
	REQUIRE(Buffer.GetSize() == 0);
	REQUIRE_FALSE(Buffer.IsOwning());
}

TEST_CASE("FDataBuffer parameterized constructor", "[DataBuffer]")
{
	SECTION("Non-owning Buffer")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(10));
		FDataBuffer<uint8> Buffer(Data, 10, false);

		REQUIRE(Buffer.IsValid());
		REQUIRE_FALSE(Buffer.IsEmpty());
		REQUIRE(Buffer.GetData() == Data);
		REQUIRE(Buffer.GetSize() == 10);
		REQUIRE_FALSE(Buffer.IsOwning());

		FMemory::Free(Data);
	}

	SECTION("Owning Buffer")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(10));
		FDataBuffer<uint8> Buffer(Data, 10, true);

		REQUIRE(Buffer.IsValid());
		REQUIRE_FALSE(Buffer.IsEmpty());
		REQUIRE(Buffer.GetData() == Data);
		REQUIRE(Buffer.GetSize() == 10);
		REQUIRE(Buffer.IsOwning());
	}
}

TEST_CASE("FDataBuffer with different types", "[DataBuffer]")
{
	SECTION("Integer Buffer")
	{
		int* Data = static_cast<int*>(FMemory::Allocate(5 * sizeof(int)));
		const FDataBuffer<int> Buffer(Data, 5, true);

		REQUIRE(Buffer.GetSize() == 5);
		REQUIRE(Buffer.GetData() == Data);
	}

	SECTION("Float Buffer")
	{
		float* Data = static_cast<float*>(FMemory::Allocate(3 * sizeof(float)));
		const FDataBuffer<float> Buffer(Data, 3, true);

		REQUIRE(Buffer.GetSize() == 3);
		REQUIRE(Buffer.GetData() == Data);
	}
}

TEST_CASE("FDataBuffer Fill method", "[DataBuffer]")
{
	SECTION("Fill with default value")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(10));
		const FDataBuffer<uint8> Buffer(Data, 10, true);

		Buffer.Fill();

		for (size64 Index = 0; Index < Buffer.GetSize(); ++Index)
		{
			REQUIRE(Buffer.GetData()[Index] == 0);
		}
	}

	SECTION("Fill with custom value")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(10));
		const FDataBuffer<uint8> Buffer(Data, 10, true);

		Buffer.Fill(0xAB);

		for (size64 Index = 0; Index < Buffer.GetSize(); ++Index)
		{
			REQUIRE(Buffer.GetData()[Index] == 0xAB);
		}
	}

	SECTION("Fill invalid Buffer")
	{
		const FDataBuffer<uint8> Buffer;

		Buffer.Fill(0xFF);

		REQUIRE_FALSE(Buffer.IsValid());
	}
}

TEST_CASE("FDataBuffer Resize method", "[DataBuffer]")
{
	SECTION("Resize to larger size")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(5));
		for (int Index = 0; Index < 5; Index++)
		{
			Data[Index] = static_cast<uint8>(Index + 1);
		}

		FDataBuffer<uint8> Buffer(Data, 5, true);
		Buffer.Resize(10);

		REQUIRE(Buffer.GetSize() == 10);
		REQUIRE(Buffer.GetData() != Data);

		for (int Index = 0; Index < 5; Index++)
		{
			REQUIRE(Buffer.GetData()[Index] == Index + 1);
		}
	}

	SECTION("Resize to smaller size")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(10));
		for (int Index = 0; Index < 10; Index++)
		{
			Data[Index] = static_cast<uint8>(Index + 1);
		}

		FDataBuffer<uint8> Buffer(Data, 10, true);
		Buffer.Resize(5);

		REQUIRE(Buffer.GetSize() == 5);
		REQUIRE(Buffer.GetData() != Data);

		for (int Index = 0; Index < 5; Index++)
		{
			REQUIRE(Buffer.GetData()[Index] == Index + 1);
		}
	}

	SECTION("Resize non-owning Buffer")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(5));
		for (int Index = 0; Index < 5; Index++)
		{
			Data[Index] = static_cast<uint8>(Index + 1);
		}

		FDataBuffer<uint8> Buffer(Data, 5, false);
		Buffer.Resize(10);

		REQUIRE(Buffer.GetSize() == 10);
		REQUIRE(Buffer.GetData() != Data);
		REQUIRE_FALSE(Buffer.IsOwning());

		for (int Index = 0; Index < 5; Index++)
		{
			REQUIRE(Buffer.GetData()[Index] == Index + 1);
		}
		FMemory::Free(Data);
	}
}

TEST_CASE("FDataBuffer CreateView method", "[DataBuffer]")
{
	uint8* Data = static_cast<uint8*>(FMemory::Allocate(10));
	for (int Index = 0; Index < 10; Index++)
	{
		Data[Index] = static_cast<uint8>(Index);
	}

	FDataBuffer<uint8> Buffer(Data, 10, true);

	SECTION("Full View")
	{
		auto View = Buffer.CreateView();

		REQUIRE(View.GetSize() == 10);
		REQUIRE(View.GetData() == Data);
		REQUIRE_FALSE(View.IsOwning());
	}

	SECTION("Partial View with offset")
	{
		auto View = Buffer.CreateView(sizeof(uint8) + 4);

		REQUIRE(View.GetSize() == 5);
		REQUIRE(View.GetData() == Data + 5);
		REQUIRE_FALSE(View.IsOwning());

		REQUIRE(View.GetData()[0] == 5);
	}

	SECTION("Partial View with max size")
	{
		auto View = Buffer.CreateView(0, 5);

		REQUIRE(View.GetSize() == 5);
		REQUIRE(View.GetData() == Data);
		REQUIRE_FALSE(View.IsOwning());
	}

	SECTION("Partial View with offset and max size")
	{
		auto View = Buffer.CreateView(3, 4);

		REQUIRE(View.GetSize() == 4);
		REQUIRE(View.GetData() == Data + 3);
		REQUIRE_FALSE(View.IsOwning());

		REQUIRE(View.GetData()[0] == 3);
	}
}

TEST_CASE("FDataBuffer CreateCopy method", "[DataBuffer]")
{
	uint8* Data = static_cast<uint8*>(FMemory::Allocate(10));
	for (int Index = 0; Index < 10; Index++)
	{
		Data[Index] = static_cast<uint8>(Index);
	}

	FDataBuffer<uint8> Buffer(Data, 10, true);

	SECTION("Full Copy")
	{
		auto Copy = Buffer.CreateCopy();

		REQUIRE(Copy.GetSize() == 10);
		REQUIRE(Copy.GetData() != Data);
		REQUIRE(Copy.IsOwning());

		for (int Index = 0; Index < 10; Index++)
		{
			REQUIRE(Copy.GetData()[Index] == Index);
		}
	}

	SECTION("Partial Copy with offset")
	{
		auto Copy = Buffer.CreateCopy(5);

		REQUIRE(Copy.GetSize() == 5);
		REQUIRE(Copy.GetData() != Data);
		REQUIRE(Copy.IsOwning());

		for (int Index = 0; Index < 5; Index++)
		{
			REQUIRE(Copy.GetData()[Index] == Index + 5);
		}
	}

	SECTION("Partial Copy with max size")
	{
		auto Copy = Buffer.CreateCopy(0, 5);

		REQUIRE(Copy.GetSize() == 5);
		REQUIRE(Copy.GetData() != Data);
		REQUIRE(Copy.IsOwning());

		for (int Index = 0; Index < 5; Index++)
		{
			REQUIRE(Copy.GetData()[Index] == Index);
		}
	}

	SECTION("Partial Copy with offset and max size")
	{
		auto Copy = Buffer.CreateCopy(3, 4);

		REQUIRE(Copy.GetSize() == 4);
		REQUIRE(Copy.GetData() != Data);
		REQUIRE(Copy.IsOwning());

		for (int Index = 0; Index < 4; Index++)
		{
			REQUIRE(Copy.GetData()[Index] == Index + 3);
		}
	}
}

TEST_CASE("FDataBuffer AsString method", "[DataBuffer]")
{
	SECTION("Valid string Data")
	{
		const char* testString = "Hello, World!";
		const size64 stringSize = strlen(testString);

		uint8* Data = static_cast<uint8*>(FMemory::Allocate(stringSize));
		FMemory::Copy(Data, testString, stringSize);

		const FDataBuffer<uint8> Buffer(Data, stringSize, true);

		FString Result = Buffer.AsString();
	}

	SECTION("Invalid Buffer")
	{
		const FDataBuffer<uint8> Buffer;

		FString Result = Buffer.AsString();
	}
}

TEST_CASE("FDataBuffer Copy and move operations", "[DataBuffer]")
{
	SECTION("Copy constructor")
	{
		uint8* Data = static_cast<uint8*>(FMemory::Allocate(5));
		for (int Index = 0; Index < 5; Index++)
		{
			Data[Index] = static_cast<uint8>(Index + 1);
		}

		FDataBuffer<uint8> Original(Data, 5, true);
		FDataBuffer<uint8> Copy(Original);

		REQUIRE(Copy.GetSize() == 5);
		REQUIRE(Copy.GetData() == Data);
		REQUIRE(Copy.IsOwning() != Original.IsOwning());
	}

	SECTION("Copy assignment")
	{
		uint8* Data1 = static_cast<uint8*>(FMemory::Allocate(5));
		uint8* Data2 = static_cast<uint8*>(FMemory::Allocate(3));

		FDataBuffer<uint8> Buffer1(Data1, 5, true);
		FDataBuffer<uint8> Buffer2(Data2, 3, true);

		Buffer2 = Buffer1;

		REQUIRE(Buffer2.GetSize() == 5);
		REQUIRE(Buffer2.GetData() == Data1);
		REQUIRE(Buffer2.IsOwning() != Buffer1.IsOwning());
	}
}
