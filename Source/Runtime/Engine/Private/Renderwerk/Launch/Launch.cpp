#include "pch.hpp"

#include "Renderwerk/Launch/Launch.hpp"

#include "Renderwerk/Core/Error/ErrorHandler.hpp"

void GuardedMain()
{
}

int32 Launch()
{
	SetUnhandledExceptionFilter(FErrorHandler::Handle);
	__try
	{
		GuardedMain();
	}
	__except (FErrorHandler::Handle(GetExceptionInformation()))
	{
	}
	return FErrorHandler::GetExitCode();
}
