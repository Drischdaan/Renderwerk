#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

DECLARE_LOG_CATEGORY(LogPlatform, Trace);

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
};

class RENDERWERK_API FPlatform
{
public:
	NODISCARD static uint64 GetCurrentThreadId();

public:
	NODISCARD static const FProcessorInfo& GetProcessorInfo() { return ProcessorInfo; }
	NODISCARD static const FMemoryInfo& GetMemoryInfo() { return MemoryInfo; }

private:
	static void Initialize();
	static void Shutdown();

private:
	static FString QueryCPUName();
	static uint32 QueryPhysicalCoreCount();

private:
	static FProcessorInfo ProcessorInfo;
	static FMemoryInfo MemoryInfo;

	friend void InitializeCore();
	friend void ShutdownCore();
};
