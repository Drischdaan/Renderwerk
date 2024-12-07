#include "pch.h"

#include "Renderwerk/Launch/Launch.h"

int32 LaunchEngine([[maybe_unused]] const FLaunchParameters& Parameters)
{
	TVector<FString> Arguments(Parameters.ArgumentCount);
	for (uint32 Index = 1; Index < Parameters.ArgumentCount; ++Index) // Skip the first argument, which is the executable path
		Arguments[Index] = Parameters.Arguments[Index];
	return EXIT_SUCCESS;
}
