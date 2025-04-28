#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/String.hpp"

struct ENGINE_API FTagComponent
{
	FAnsiString Name;

	explicit FTagComponent(const FAnsiString& Name)
		: Name(Name)
	{
	}
};
