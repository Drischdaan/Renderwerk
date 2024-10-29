#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Engine/System.h"

class RENDERWERK_API FSystemManager
{
public:
	INLINE static void Clear()
	{
		for (auto Iterator = Systems.rbegin(); Iterator != Systems.rend(); ++Iterator)
		{
			TSharedPtr<ISystem> System = Iterator->second;
			System->Shutdown();
			System.reset();
		}
		Systems.clear();
	}

public:
	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	INLINE static NODISCARD TSharedPtr<TSystem> Get()
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(Systems.contains(TypeName), "Subsystem not registered")
		TSharedPtr<ISystem> System = Systems.at(TypeName);
		return std::static_pointer_cast<TSystem>(System);
	}

	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	INLINE static NODISCARD bool8 IsRegistered()
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
		DEBUG_ASSERTM(!Systems.contains(TypeName), "Subsystem already registered")
		TSharedPtr<ISystem> System = MakeShared<TSystem>(std::forward<TArguments>(Arguments)...);
		Systems.insert({TypeName, System});
		System->Initialize();
	}

	template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
	INLINE static void Unregister()
	{
		const std::type_info& TypeInfo = typeid(TSystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(Systems.contains(TypeName), "Subsystem not registered")
		TSharedPtr<ISystem> System = Systems.at(TypeName);
		System->Shutdown();
		System.reset();
		Systems.erase(TypeName);
	}

private:
	INLINE static TMap<FAnsiString, TSharedPtr<ISystem>> Systems;
};

/**
 * @brief Convenience function to retrieve a system.
 */
template <typename TSystem, typename = std::is_base_of<ISystem, TSystem>>
RENDERWERK_API INLINE TSharedPtr<TSystem> GetSystem()
{
	return FSystemManager::Get<TSystem>();
}
