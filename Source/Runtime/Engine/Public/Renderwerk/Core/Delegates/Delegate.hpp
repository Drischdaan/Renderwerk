#pragma once

#include <functional>
#include <type_traits>

#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

template <typename TReturnType, typename... TParameters>
class TDelegate
{
public:
	using TFuncType = std::function<TReturnType(TParameters...)>;

public:
	TDelegate() = default;
	~TDelegate() = default;

	DEFAULT_COPY_MOVEABLE(TDelegate)

public:
	[[nodiscard]] bool8 IsBound() const
	{
		return Function != nullptr;
	}

	void Bind(TFuncType&& BoundFunction)
	{
		Function = std::move(BoundFunction);
	}

	template <typename TOwner>
	void BindRaw(TOwner* Instance, TReturnType (TOwner::*OwnerFunction)(TParameters...))
	{
		Function = [Instance, OwnerFunction](TParameters... Parameters) -> TReturnType
		{
			return (Instance->*OwnerFunction)(std::forward<TParameters>(Parameters)...);
		};
	}

	template <typename TOwner>
	void BindRaw(TOwner* Owner, TReturnType (TOwner::*OwnerFunction)(TParameters...) const)
	{
		Function = [Owner, OwnerFunction](TParameters... Parameters) -> TReturnType
		{
			return (Owner->*OwnerFunction)(std::forward<TParameters>(Parameters)...);
		};
	}

	void Unbind()
	{
		Function = nullptr;
	}

	template <typename... TOtherParameters>
		requires (sizeof...(TOtherParameters) == sizeof...(TParameters) && (std::convertible_to<TOtherParameters&&, TParameters> && ...))
	TReturnType Execute(TOtherParameters&&... Parameters)
	{
		if constexpr (std::is_same_v<TReturnType, void>)
		{
			Function.operator()(std::forward<TOtherParameters>(Parameters)...);
			return;
		}
		return Function.operator()(std::forward<TOtherParameters>(Parameters)...);
	}

	template <typename... TOtherParameters>
		requires (sizeof...(TOtherParameters) == sizeof...(TParameters) && (std::convertible_to<TOtherParameters&&, TParameters> && ...))
	TReturnType ExecuteIfBound(TOtherParameters&&... Parameters) requires std::is_same_v<void, TReturnType> || std::is_default_constructible_v<TReturnType>
	{
		if (!IsBound())
		{
			if constexpr (std::is_same_v<TReturnType, void>)
			{
				return;
			}
			return TReturnType();
		}
		return Execute(std::forward<TOtherParameters>(Parameters)...);
	}

private:
	TFuncType Function = nullptr;
};
