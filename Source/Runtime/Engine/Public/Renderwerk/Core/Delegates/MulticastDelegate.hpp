#pragma once

#include <functional>
#include <ranges>
#include <type_traits>

#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/Map.hpp"
#include "Renderwerk/Core/Logging/Logger.hpp"

#define RW_INVALID_DELEGATE_HANDLE_ID FORWARD(0)

struct FDelegateHandle
{
public:
	constexpr FDelegateHandle() = default;

	constexpr FDelegateHandle(const uint64 InId)
		: Id(InId)
	{
	}

	constexpr ~FDelegateHandle() = default;

	DEFAULT_COPY_MOVEABLE_PREFIX(FDelegateHandle, constexpr)

public:
	[[nodiscard]] constexpr bool8 IsValid() const
	{
		return Id != RW_INVALID_DELEGATE_HANDLE_ID;
	}

public:
	[[nodiscard]] constexpr uint64 GetId() const { return Id; }

public:
	constexpr operator uint64() const
	{
		return GetId();
	}

	constexpr operator bool8() const
	{
		return IsValid();
	}

private:
	uint64 Id = RW_INVALID_DELEGATE_HANDLE_ID;
};

template <typename... TParameters>
class TMulticastDelegate
{
public:
	using TFuncType = std::function<void(TParameters...)>;

public:
	TMulticastDelegate() = default;
	~TMulticastDelegate() = default;

	NON_COPY_MOVEABLE(TMulticastDelegate)

public:
	[[nodiscard]] bool8 IsBound()
	{
		return !Functions.empty();
	}

	[[nodiscard]] FDelegateHandle Bind(TFuncType&& Function)
	{
		const FDelegateHandle Handle(GetNextHandleId());
		Functions.insert(std::make_pair(Handle.GetId(), std::move(Function)));
		return Handle;
	}

	template <typename TOwner>
	[[nodiscard]] FDelegateHandle BindRaw(TOwner* Owner, void (TOwner::*Function)(TParameters...))
	{
		return Bind([Owner, Function](TParameters... Args)
		{
			(Owner->*Function)(std::forward<TParameters>(Args)...);
		});
	}

	template <typename TOwner>
	[[nodiscard]] FDelegateHandle BindRaw(const TOwner* Owner, void (TOwner::*Function)(TParameters...) const)
	{
		return Bind([Owner, Function](TParameters... Args)
		{
			(Owner->*Function)(std::forward<TParameters>(Args)...);
		});
	}

	bool8 Unbind(const FDelegateHandle& Handle)
	{
		if (!Functions.contains(Handle.GetId()))
		{
			RW_LOG(Warning, "There is no handle with id '{}' registered on this delegate", Handle.GetId());
			return false;
		}
		Functions.erase(Handle);
		return true;
	}

	void Clear()
	{
		Functions.clear();
	}

	template <typename... TOtherParameters>
		requires (sizeof...(TOtherParameters) == sizeof...(TParameters) && (std::convertible_to<TOtherParameters&&, TParameters> && ...))
	void Broadcast(TOtherParameters&&... Parameters)
	{
		for (TFuncType& Function : Functions | std::views::values)
		{
			Function.operator()(std::forward<TOtherParameters>(Parameters)...);
		}
	}

private:
	[[nodiscard]] uint64 GetNextHandleId()
	{
		++NextHandleId;
		if (NextHandleId == RW_INVALID_DELEGATE_HANDLE_ID)
		{
			++NextHandleId;
		}
		return NextHandleId;
	}

private:
	TMap<uint64, TFuncType> Functions;
	uint64 NextHandleId;
};
