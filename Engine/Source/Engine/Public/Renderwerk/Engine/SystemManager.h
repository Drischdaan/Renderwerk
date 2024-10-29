#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Engine/System.h"

#include <ranges>

/**
 * Manages all registered systems.
 */
class RENDERWERK_API FSystemManger
{
public:
	/**
	 * Clears all registered systems and shuts them down.
	 */
	INLINE static void Clear()
	{
		for (TSharedPtr<ISystem>& System : Systems | std::views::values)
		{
			System->Shutdown();
			System.reset();
		}
		Systems.clear();
	}

public:
	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	NODISCARD INLINE static TSharedPtr<TSystem> Get()
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(Systems.contains(TypeName), "System not registered");
		TSharedPtr<ISystem> System = Systems.at(TypeName);
		return std::static_pointer_cast<TSystem>(System);
	}

	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	NODISCARD INLINE static bool8 IsRegistered()
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		return Systems.contains(TypeName);
	}

	template <typename TSystem, typename... TArguments, typename = std::is_base_of<ISystem, TSystem>>
	INLINE static void Register(TArguments&&... Arguments)
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(!Systems.contains(TypeName), "System already registered");
		TSharedPtr<ISystem> System = MakeShared<TSystem>(std::forward<TArguments>(Arguments)...);
		Systems.insert({TypeName, System});
		System->Initialize();
	}

	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	INLINE static void Unregister()
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(Systems.contains(TypeName), "System not registered");
		TSharedPtr<ISystem> System = Systems.at(TypeName);
		System->Shutdown();
		System.reset();
		Systems.erase(TypeName);
	}

private:
	INLINE static TMap<FAnsiString, TSharedPtr<ISystem>> Systems;
};

/**
 * Convenience function to get a system.
 * @tparam TSystem The system type.
 * @return The system.
 */
template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
RENDERWERK_API INLINE TSharedPtr<TSystem> GetSystem()
{
	return FSystemManger::Get<TSystem>();
}
