#include "catch_amalgamated.hpp"

#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"

class FMockObject
{
public:
	void SimpleFunction(const int32 Value)
	{
		LastValue = Value;
		CallCount++;
	}

	void FunctionWithMultipleParams(const int32 A, const float32 B, const bool8 C)
	{
		LastValue = A;
		LastFloat = B;
		LastBool = C;
		CallCount++;
	}

	void ConstFunction(const int32 Value) const
	{
		LastConstValue = Value;
		ConstCallCount++;
	}

	int32 GetCallCount() const { return CallCount; }
	int32 GetConstCallCount() const { return ConstCallCount; }
	int32 GetLastValue() const { return LastValue; }
	float32 GetLastFloat() const { return LastFloat; }
	bool8 GetLastBool() const { return LastBool; }
	int32 GetLastConstValue() const { return LastConstValue; }

	void ResetCounters()
	{
		CallCount = 0;
		ConstCallCount = 0;
		LastValue = 0;
		LastFloat = 0.0f;
		LastBool = false;
		LastConstValue = 0;
	}

private:
	mutable int32 CallCount = 0;
	mutable int32 ConstCallCount = 0;
	int32 LastValue = 0;
	float32 LastFloat = 0.0f;
	bool8 LastBool = false;
	mutable int32 LastConstValue = 0;
};

TEST_CASE("FDelegateHandle functionality", "[delegate]")
{
	SECTION("Default constructor creates invalid handle")
	{
		FDelegateHandle Handle;
		REQUIRE_FALSE(Handle.IsValid());
		REQUIRE(Handle.GetId() == RW_INVALID_DELEGATE_HANDLE_ID);
	}

	SECTION("Construction with valid ID")
	{
		FDelegateHandle Handle(42);
		REQUIRE(Handle.IsValid());
		REQUIRE(Handle.GetId() == 42);
	}

	SECTION("Boolean conversion operator")
	{
		FDelegateHandle InvalidHandle;
		FDelegateHandle ValidHandle(123);

		REQUIRE_FALSE(bool8(InvalidHandle));
		REQUIRE(bool8(ValidHandle));
	}

	SECTION("Integer conversion operator")
	{
		FDelegateHandle Handle(456);
		const uint64 Id = Handle;
		REQUIRE(Id == 456);
	}

	SECTION("Copy construction and assignment")
	{
		FDelegateHandle Original(789);

		FDelegateHandle CopyCtor(Original);
		REQUIRE(CopyCtor.GetId() == Original.GetId());

		FDelegateHandle CopyAssign;
		CopyAssign = Original;
		REQUIRE(CopyAssign.GetId() == Original.GetId());
	}

	SECTION("Move construction and assignment")
	{
		FDelegateHandle MoveCtor(FDelegateHandle(102));
		REQUIRE(MoveCtor.GetId() == 102);

		FDelegateHandle MoveAssign;
		MoveAssign = FDelegateHandle(103);
		REQUIRE(MoveAssign.GetId() == 103);
	}
}

TEST_CASE("TMulticastDelegate basic functionality", "[delegate]")
{
	SECTION("Default constructor creates unbound delegate")
	{
		TMulticastDelegate<int32> Delegate;
		REQUIRE_FALSE(Delegate.IsBound());
	}

	SECTION("Binding a lambda function")
	{
		TMulticastDelegate<int32> Delegate;
		int32 CallCount = 0;

		FDelegateHandle Handle = Delegate.Bind([&CallCount](const int32 Value)
		{
			CallCount += Value;
		});

		REQUIRE(Delegate.IsBound());
		REQUIRE(Handle.IsValid());

		Delegate.Broadcast(5);
		REQUIRE(CallCount == 5);

		Delegate.Broadcast(3);
		REQUIRE(CallCount == 8);
	}

	SECTION("Binding multiple functions")
	{
		TMulticastDelegate<int32> Delegate;
		int32 Sum = 0;
		int32 Product = 1;

		FDelegateHandle Handle1 = Delegate.Bind([&Sum](const int32 Value)
		{
			Sum += Value;
		});

		FDelegateHandle Handle2 = Delegate.Bind([&Product](const int32 Value)
		{
			Product *= Value;
		});

		REQUIRE(Delegate.IsBound());
		REQUIRE(Handle1.IsValid());
		REQUIRE(Handle2.IsValid());
		REQUIRE(Handle1.GetId() != Handle2.GetId());

		Delegate.Broadcast(5);
		REQUIRE(Sum == 5);
		REQUIRE(Product == 5);

		Delegate.Broadcast(3);
		REQUIRE(Sum == 8);
		REQUIRE(Product == 15);
	}

	SECTION("Unbinding a function")
	{
		TMulticastDelegate<int32> Delegate;
		int32 Value1 = 0;
		int32 Value2 = 0;

		FDelegateHandle Handle1 = Delegate.Bind([&Value1](const int32 Val) { Value1 = Val; });
		FDelegateHandle Handle2 = Delegate.Bind([&Value2](const int32 Val) { Value2 = Val; });

		bool8 UnbindResult = Delegate.Unbind(Handle1);
		REQUIRE(UnbindResult);
		REQUIRE(Delegate.IsBound());

		Delegate.Broadcast(42);
		REQUIRE(Value1 == 0);
		REQUIRE(Value2 == 42);

		UnbindResult = Delegate.Unbind(Handle2);
		REQUIRE(UnbindResult);
		REQUIRE_FALSE(Delegate.IsBound());

		UnbindResult = Delegate.Unbind(Handle1);
		REQUIRE_FALSE(UnbindResult);
	}

	SECTION("Clearing all bindings")
	{
		TMulticastDelegate<int32> Delegate;
		int32 Value1 = 0;
		int32 Value2 = 0;

		FDelegateHandle Handle1 = Delegate.Bind([&Value1](const int32 Val) { Value1 = Val; });
		FDelegateHandle Handle2 = Delegate.Bind([&Value2](const int32 Val) { Value2 = Val; });

		REQUIRE(Delegate.IsBound());

		Delegate.Clear();
		REQUIRE_FALSE(Delegate.IsBound());

		Delegate.Broadcast(42);
		REQUIRE(Value1 == 0);
		REQUIRE(Value2 == 0);

		Delegate.Unbind(Handle2);
		Delegate.Unbind(Handle1);
	}
}

TEST_CASE("TMulticastDelegate with member functions", "[delegate]")
{
	FMockObject MockObject;

	SECTION("Binding a non-const member function")
	{
		TMulticastDelegate<int32> Delegate;
		FDelegateHandle Handle = Delegate.BindRaw(&MockObject, &FMockObject::SimpleFunction);

		REQUIRE(Handle.IsValid());
		REQUIRE(Delegate.IsBound());

		Delegate.Broadcast(42);
		REQUIRE(MockObject.GetCallCount() == 1);
		REQUIRE(MockObject.GetLastValue() == 42);

		Delegate.Broadcast(123);
		REQUIRE(MockObject.GetCallCount() == 2);
		REQUIRE(MockObject.GetLastValue() == 123);
	}

	SECTION("Binding a const member function")
	{
		constexpr FMockObject ConstMockObject;
		TMulticastDelegate<int32> Delegate;

		FDelegateHandle Handle = Delegate.BindRaw(&ConstMockObject, &FMockObject::ConstFunction);

		REQUIRE(Handle.IsValid());
		REQUIRE(Delegate.IsBound());

		Delegate.Broadcast(42);
		REQUIRE(ConstMockObject.GetConstCallCount() == 1);
		REQUIRE(ConstMockObject.GetLastConstValue() == 42);
	}

	SECTION("Binding member functions with multiple parameters")
	{
		TMulticastDelegate<int32, float32, bool8> Delegate;
		FDelegateHandle Handle = Delegate.BindRaw(&MockObject, &FMockObject::FunctionWithMultipleParams);

		REQUIRE(Handle.IsValid());
		REQUIRE(Delegate.IsBound());

		Delegate.Broadcast(42, 3.14f, true);
		REQUIRE(MockObject.GetCallCount() == 1);
		REQUIRE(MockObject.GetLastValue() == 42);
		REQUIRE(std::abs(MockObject.GetLastFloat() - 3.14f) < 0.0001f);
		REQUIRE(MockObject.GetLastBool() == true);
	}
}

TEST_CASE("TMulticastDelegate parameter forwarding", "[delegate]")
{
	SECTION("Forwarding reference parameters")
	{
		TMulticastDelegate<int32&> Delegate;

		FDelegateHandle Handle = Delegate.Bind([](int32& Value)
		{
			Value *= 2;
		});

		int32 TestValue = 10;
		Delegate.Broadcast(TestValue);
		REQUIRE(TestValue == 20);

		Delegate.Broadcast(TestValue);
		REQUIRE(TestValue == 40);

		Delegate.Unbind(Handle);
	}

	SECTION("Forwarding mixed parameters")
	{
		TMulticastDelegate<int32, int32&, const int32&> Delegate;

		FDelegateHandle Handle = Delegate.Bind([](const int32 ByValue, int32& ByRef, const int32& ByConstRef)
		{
			ByRef = ByValue + ByConstRef;
		});

		int32 Value1 = 10;
		int32 Value2 = 0;
		constexpr int32 Value3 = 5;

		Delegate.Broadcast(Value1, Value2, Value3);
		REQUIRE(Value2 == 15);

		Delegate.Broadcast(20, Value2, Value3);
		REQUIRE(Value2 == 25);

		Delegate.Unbind(Handle);
	}

	SECTION("Parameter type conversion")
	{
		TMulticastDelegate<int32, float32> Delegate;
		bool8 CallbackExecuted = false;

		FDelegateHandle Handle = Delegate.Bind([&CallbackExecuted](const int32 IntVal, const float32 FloatVal)
		{
			CallbackExecuted = true;
			REQUIRE(IntVal == 42);
			REQUIRE(std::abs(FloatVal - 3.14f) < 0.0001f);
		});

		int16 SmallInt = 42;
		float32 DoublePrecision = 3.14f;

		Delegate.Broadcast(SmallInt, DoublePrecision);
		REQUIRE(CallbackExecuted);

		Delegate.Unbind(Handle);
	}
}

TEST_CASE("TMulticastDelegate handle generation", "[delegate]")
{
	SECTION("Handles should be unique")
	{
		TMulticastDelegate<> Delegate;

		const FDelegateHandle Handle1 = Delegate.Bind([]()
		{
		});
		const FDelegateHandle Handle2 = Delegate.Bind([]()
		{
		});
		const FDelegateHandle Handle3 = Delegate.Bind([]()
		{
		});

		REQUIRE(Handle1.GetId() != Handle2.GetId());
		REQUIRE(Handle1.GetId() != Handle3.GetId());
		REQUIRE(Handle2.GetId() != Handle3.GetId());
	}
}

TEST_CASE("TMulticastDelegate broadcast order", "[delegate]")
{
	SECTION("Functions are called in order of binding")
	{
		TMulticastDelegate<> Delegate;
		std::vector<int32> CallOrder;

		const FDelegateHandle Handle1 = Delegate.Bind([&CallOrder]() { CallOrder.push_back(1); });
		const FDelegateHandle Handle2 = Delegate.Bind([&CallOrder]() { CallOrder.push_back(2); });
		const FDelegateHandle Handle3 = Delegate.Bind([&CallOrder]() { CallOrder.push_back(3); });

		Delegate.Broadcast();

		REQUIRE(CallOrder.size() == 3);
		REQUIRE(CallOrder[0] == 1);
		REQUIRE(CallOrder[1] == 2);
		REQUIRE(CallOrder[2] == 3);

		Delegate.Unbind(Handle3);
		Delegate.Unbind(Handle2);
		Delegate.Unbind(Handle1);
	}
}
