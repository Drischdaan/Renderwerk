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
	FSystemManger();
	~FSystemManger();

public:
	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	NODISCARD INLINE TSharedPtr<TSystem> Get()
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(Systems.contains(TypeName), "System not registered");
		TSharedPtr<ISystem> System = Systems.at(TypeName);
		return std::static_pointer_cast<TSystem>(System);
	}

	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	NODISCARD INLINE bool8 IsRegistered() const
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		return Systems.contains(TypeName);
	}

	template <typename TSystem, typename... TArguments, typename = std::is_base_of<ISystem, TSystem>>
	INLINE void Register(TArguments&&... Arguments)
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(!Systems.contains(TypeName), "System already registered");
		TSharedPtr<ISystem> System = MakeShared<TSystem>(std::forward<TArguments>(Arguments)...);
		Systems.insert({TypeName, System});
		System->Initialize();
	}

	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	INLINE void Unregister()
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
	TMap<FAnsiString, TSharedPtr<ISystem>> Systems;
};
