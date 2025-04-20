#include <catch_amalgamated.hpp>

#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Delegates/Delegate.hpp"

class MockObject
{
public:
	// ReSharper disable CppMemberFunctionMayBeStatic

	int32 Add(const int32 First, const int32 Second)
	{
		return First + Second;
	}

	int32 AddWithMultiplier(const int32 First, const int32 Second, const int32 multiplier)
	{
		return (First + Second) * multiplier;
	}

	int32 GetConstValue(const int32 Value) const
	{
		return Value * 2;
	}

	void DoSomething(int32& Value)
	{
		Value *= 2;
	}

	// ReSharper restore CppMemberFunctionMayBeStatic
};

TEST_CASE("TDelegate basic functionality", "[Delegate]")
{
	SECTION("Default constructed Delegate is not bound")
	{
		TDelegate<int32, int32, int32> Delegate;
		REQUIRE_FALSE(Delegate.IsBound());
	}

	SECTION("Binding First lambda function")
	{
		TDelegate<int32, int32, int32> Delegate;
		Delegate.Bind([](const int32 First, const int32 Second) { return First + Second; });

		REQUIRE(Delegate.IsBound());
		REQUIRE(Delegate.Execute(2, 3) == 5);
	}

	SECTION("Binding First std::function")
	{
		TDelegate<int32, int32, int32> Delegate;
		std::function<int32(int32, int32)> Function = [](const int32 First, const int32 Second) { return First * Second; };
		Delegate.Bind(std::move(Function));

		REQUIRE(Delegate.IsBound());
		REQUIRE(Delegate.Execute(4, 5) == 20);
	}

	SECTION("Unbinding First Delegate")
	{
		TDelegate<int32, int32, int32> Delegate;
		Delegate.Bind([](const int32 First, const int32 Second) { return First + Second; });
		REQUIRE(Delegate.IsBound());

		Delegate.Unbind();
		REQUIRE_FALSE(Delegate.IsBound());
	}

	SECTION("ExecuteIfBound with bound Delegate")
	{
		TDelegate<int32, int32, int32> Delegate;
		Delegate.Bind([](const int32 First, const int32 Second) { return First - Second; });

		REQUIRE(Delegate.ExecuteIfBound(10, 4) == 6);
	}

	SECTION("ExecuteIfBound with unbound Delegate")
	{
		TDelegate<int32, int32, int32> Delegate;

		REQUIRE(Delegate.ExecuteIfBound(10, 4) == 0);
	}
}

TEST_CASE("TDelegate with void return type", "[Delegate]")
{
	SECTION("Binding and executing void function")
	{
		TDelegate<void, int32&> Delegate;
		int32 Value = 5;

		Delegate.Bind([](int32& ParameterValue) { ParameterValue *= 3; });
		Delegate.Execute(Value);

		REQUIRE(Value == 15);
	}

	SECTION("ExecuteIfBound with void return type")
	{
		TDelegate<void, int32&> Delegate;
		int32 Value = 5;

		Delegate.ExecuteIfBound(Value);
		REQUIRE(Value == 5);

		Delegate.Bind([](int32& ParameterValue) { ParameterValue *= 3; });
		Delegate.ExecuteIfBound(Value);
		REQUIRE(Value == 15);
	}
}

TEST_CASE("TDelegate with member functions", "[Delegate]")
{
	MockObject Object;

	SECTION("Binding First non-const member function")
	{
		TDelegate<int32, int32, int32> Delegate;
		Delegate.BindRaw(&Object, &MockObject::Add);

		REQUIRE(Delegate.IsBound());
		REQUIRE(Delegate.Execute(10, 20) == 30);
	}

	SECTION("Binding First const member function")
	{
		TDelegate<int32, int32> Delegate;
		Delegate.BindRaw(&Object, &MockObject::GetConstValue);

		REQUIRE(Delegate.IsBound());
		REQUIRE(Delegate.Execute(15) == 30);
	}

	SECTION("Binding First member function with reference parameter")
	{
		TDelegate<void, int32&> Delegate;
		Delegate.BindRaw(&Object, &MockObject::DoSomething);

		int32 Value = 10;
		Delegate.Execute(Value);
		REQUIRE(Value == 20);
	}

	SECTION("Binding First member function with multiple parameters")
	{
		TDelegate<int32, int32, int32, int32> Delegate;
		Delegate.BindRaw(&Object, &MockObject::AddWithMultiplier);

		REQUIRE(Delegate.Execute(2, 3, 4) == 20);
	}
}

TEST_CASE("TDelegate copy and move semantics", "[Delegate]")
{
	SECTION("Copy constructor")
	{
		TDelegate<int32, int32> Delegate;
		Delegate.Bind([](const int32 x) { return x * 2; });

		TDelegate<int32, int32> DelegateCopy(Delegate);
		REQUIRE(DelegateCopy.IsBound());
		REQUIRE(DelegateCopy.Execute(5) == 10);
	}

	SECTION("Copy assignment")
	{
		TDelegate<int32, int32> Delegate;
		Delegate.Bind([](const int32 x) { return x * 2; });

		TDelegate<int32, int32> DelegateCopy;
		DelegateCopy = Delegate;
		REQUIRE(DelegateCopy.IsBound());
		REQUIRE(DelegateCopy.Execute(5) == 10);
	}

	SECTION("Move constructor")
	{
		TDelegate<int32, int32> Delegate;
		Delegate.Bind([](const int32 x) { return x * 2; });

		TDelegate<int32, int32> DelegateMoved(std::move(Delegate));
		REQUIRE(DelegateMoved.IsBound());
		REQUIRE(DelegateMoved.Execute(5) == 10);
	}

	SECTION("Move assignment")
	{
		TDelegate<int32, int32> Delegate;
		Delegate.Bind([](const int32 x) { return x * 2; });

		TDelegate<int32, int32> DelegateMoved;
		DelegateMoved = std::move(Delegate);
		REQUIRE(DelegateMoved.IsBound());
		REQUIRE(DelegateMoved.Execute(5) == 10);
	}
}
