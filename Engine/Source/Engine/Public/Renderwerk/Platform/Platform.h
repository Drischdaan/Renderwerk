#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

struct RENDERWERK_API FProcessorInfo
{
	uint32 PhysicalCoreCount = 0;
	uint32 LogicalCoreCount = 0;
	bool bIs64Bit = false;
	FString Name;
};

struct RENDERWERK_API FMemoryInfo
{
	uint64 TotalPhysicalMemory = 0;
	uint64 FreePhysicalMemory = 0;
};

class RENDERWERK_API FPlatform
{
public:
	static void Initialize();
	static void Shutdown();

public:
	NODISCARD static uint64 GetCurrentThreadId();

public:
	NODISCARD static const FProcessorInfo& GetProcessorInfo() { return ProcessorInfo; }
	NODISCARD static const FMemoryInfo& GetMemoryInfo() { return MemoryInfo; }

private:
	static FString QueryCPUName();
	static uint32 QueryPhysicalCoreCount();

private:
	static FProcessorInfo ProcessorInfo;
	static FMemoryInfo MemoryInfo;
};
