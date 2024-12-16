#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include <shaderc/shaderc.hpp>

DECLARE_LOG_CHANNEL(LogShaderCompiler);

enum class ENGINE_API EVulkanShaderStage : uint8
{
	None = 0,
	Vertex,
	Fragment,
	Compute,
	Geometry,
	Mesh,
};

struct ENGINE_API FVulkanShaderCompilationDesc
{
	FString SourceFile;
	EVulkanShaderStage Stage;
	shaderc::CompileOptions Options;
};

class ENGINE_API FVulkanShaderCompiler
{
public:
	FVulkanShaderCompiler();
	~FVulkanShaderCompiler();

	DELETE_COPY_AND_MOVE(FVulkanShaderCompiler);

public:
	[[nodiscard]] TVector<uint32> CompileShader(const FVulkanShaderCompilationDesc& Description) const;

private:
	TVector<char> PreprocessShader(const FStringView& SourceContent, const FVulkanShaderCompilationDesc& Description) const;
	TVector<char> CompileToAssembly(const TVector<char>& PreprocessedSource, const FVulkanShaderCompilationDesc& Description) const;
	TVector<uint32> AssembleToSpirv(const TVector<char>& Assembly, const FVulkanShaderCompilationDesc& Description) const;

private:
	shaderc::Compiler Compiler;
};
