#include "pch.h"

#include "Renderwerk/Engine/SystemManager.h"

FSystemManger::FSystemManger()
{
}

FSystemManger::~FSystemManger()
{
	for (TSharedPtr<ISystem>& System : Systems | std::views::values)
	{
		System->Shutdown();
		System.reset();
	}
	Systems.clear();
}
