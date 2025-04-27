#include "catch_amalgamated.hpp"

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Memory/PoolAllocator.hpp"

class FTestObject
{
public:
	FTestObject()
		: Value(0), bIsConstructed(true)
	{
	}

	FTestObject(const int32 InValue)
		: Value(InValue), bIsConstructed(true)
	{
	}

	~FTestObject()
	{
		bIsConstructed = false;
	}

	int32 GetValue() const { return Value; }
	bool8 IsConstructed() const { return bIsConstructed; }

	static int32 InstanceCount;
	static void ResetInstanceCount() { InstanceCount = 0; }

	FTestObject(const FTestObject& other) : Value(other.Value), bIsConstructed(true)
	{
		InstanceCount++;
	}

	FTestObject& operator=(const FTestObject& other)
	{
		Value = other.Value;
		return *this;
	}

private:
	int32 Value;
	bool8 bIsConstructed;
};

int32 FTestObject::InstanceCount = 0;

class FResourceHolder
{
public:
	FResourceHolder()
	{
		Resource = new int32(42);
	}

	~FResourceHolder()
	{
		delete Resource;
	}

	int32 GetValue() const { return *Resource; }

private:
	int32* Resource;
};

TEST_CASE("TPoolAllocator basic functionality", "[Pool_allocator]")
{
	constexpr size64 Capacity = 10;
	TPoolAllocator<FTestObject> Pool(Capacity);

	SECTION("Initial state")
	{
		REQUIRE(Pool.GetCapacity() == Capacity);
		REQUIRE(Pool.GetUsage() == 0);
	}

	SECTION("Allocation and deallocation")
	{
		FTestObject* Object = Pool.Allocate();
		REQUIRE(Object != nullptr);
		REQUIRE(Pool.GetUsage() == 1);

		Pool.Free(Object);
		REQUIRE(Pool.GetUsage() == 0);
	}

	SECTION("Multiple allocations")
	{
		TVector<FTestObject*> Objects;

		for (size64 Index = 0; Index < Capacity; ++Index)
		{
			Objects.push_back(Pool.Allocate());
		}

		REQUIRE(Pool.GetUsage() == Capacity);

		for (auto* Object : Objects)
		{
			Pool.Free(Object);
		}

		REQUIRE(Pool.GetUsage() == 0);
	}
}

TEST_CASE("TPoolAllocator construction and destruction", "[Pool_allocator]")
{
	constexpr size64 Capacity = 5;
	TPoolAllocator<FTestObject> Pool(Capacity);

	SECTION("Default construction")
	{
		FTestObject* Object = Pool.NewInstance();
		REQUIRE(Object != nullptr);
		REQUIRE(Object->IsConstructed());
		REQUIRE(Object->GetValue() == 0);

		Pool.DestroyInstance(Object);
		REQUIRE(Pool.GetUsage() == 0);
	}

	SECTION("Construction with arguments")
	{
		constexpr int32 testValue = 42;
		FTestObject* Object = Pool.NewInstance(testValue);

		REQUIRE(Object != nullptr);
		REQUIRE(Object->IsConstructed());
		REQUIRE(Object->GetValue() == testValue);

		Pool.DestroyInstance(Object);
	}

	SECTION("Multiple constructions and destructions")
	{
		TVector<FTestObject*> Objects;

		for (size64 Index = 0; Index < Capacity; ++Index)
		{
			Objects.push_back(Pool.NewInstance(static_cast<int32>(Index)));
			REQUIRE(static_cast<uint32>(Objects.back()->GetValue()) == Index);
		}

		for (auto it = Objects.rbegin(); it != Objects.rend(); ++it)
		{
			Pool.DestroyInstance(*it);
		}

		REQUIRE(Pool.GetUsage() == 0);
	}
}

TEST_CASE("TPoolAllocator reuse of freed memory", "[Pool_allocator]")
{
	constexpr size64 Capacity = 3;
	TPoolAllocator<FTestObject> Pool(Capacity);

	FTestObject* Object1 = Pool.Allocate();
	FTestObject* Object2 = Pool.Allocate();

	Pool.Free(Object1);

	FTestObject* Object3 = Pool.Allocate();

	REQUIRE(Object3 == Object1);

	Pool.Free(Object2);
	Pool.Free(Object3);
}

TEST_CASE("TPoolAllocator null point32er handling", "[Pool_allocator]")
{
	TPoolAllocator<FTestObject> Pool(5);

	SECTION("Free null point32er")
	{
		Pool.Free(nullptr);
		REQUIRE(Pool.GetUsage() == 0);
	}

	SECTION("DestroyInstance null point32er")
	{
		Pool.DestroyInstance(nullptr);
		REQUIRE(Pool.GetUsage() == 0);
	}
}

TEST_CASE("TPoolAllocator with Resource-owning Objects", "[Pool_allocator]")
{
	constexpr size64 Capacity = 5;
	TPoolAllocator<FResourceHolder> Pool(Capacity);

	SECTION("Proper construction and destruction")
	{
		FResourceHolder* Resource = Pool.NewInstance();
		REQUIRE(Resource->GetValue() == 42);

		Pool.DestroyInstance(Resource);
		REQUIRE(Pool.GetUsage() == 0);
	}

	SECTION("Multiple Resource Objects")
	{
		TVector<FResourceHolder*> Resources;

		for (size64 Index = 0; Index < Capacity; ++Index)
		{
			Resources.push_back(Pool.NewInstance());
		}

		for (auto* res : Resources)
		{
			Pool.DestroyInstance(res);
		}

		REQUIRE(Pool.GetUsage() == 0);
	}
}

TEST_CASE("TPoolAllocator at Capacity behavior", "[Pool_allocator]")
{
	constexpr size64 Capacity = 2;
	TPoolAllocator<FTestObject> Pool(Capacity);

	FTestObject* Object1 = Pool.Allocate();
	FTestObject* Object2 = Pool.Allocate();

	REQUIRE(Pool.GetUsage() == Capacity);

	Pool.Free(Object1);
	REQUIRE(Pool.GetUsage() == 1);

	FTestObject* Object3 = Pool.Allocate();
	REQUIRE(Pool.GetUsage() == Capacity);

	REQUIRE(Object3 == Object1);

	// Clean up
	Pool.Free(Object2);
	Pool.Free(Object3);
}

TEST_CASE("TPoolAllocator LIFO behavior", "[Pool_allocator]")
{
	constexpr size64 Capacity = 3;
	TPoolAllocator<int32> Pool(Capacity);

	int32* First = Pool.Allocate();
	int32* Second = Pool.Allocate();
	int32* Third = Pool.Allocate();

	Pool.Free(Second);
	Pool.Free(First);

	int32* Fourth = Pool.Allocate();
	REQUIRE(Fourth == First);

	int32* Fifth = Pool.Allocate();
	REQUIRE(Fifth == Second);

	Pool.Free(Third);
	Pool.Free(Fourth);
	Pool.Free(Fifth);
}

TEST_CASE("TPoolAllocator no memory leaks", "[Pool_allocator]")
{
	{
		constexpr size64 Capacity = 100;
		TPoolAllocator<FResourceHolder> Pool(Capacity);

		TVector<FResourceHolder*> Resources;
		for (size64 Index = 0; Index < Capacity; ++Index)
		{
			Resources.push_back(Pool.NewInstance());
		}

		for (size64 Index = 0; Index < Capacity / 2; ++Index)
		{
			Pool.DestroyInstance(Resources[Index]);
		}
	}
}
