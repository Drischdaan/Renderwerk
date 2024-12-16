#include "pch.h"

#include "Renderwerk/Graphics/VulkanShaderCompiler.h"

#include "Renderwerk/Platform/Filesystem.h"

DEFINE_LOG_CHANNEL(LogShaderCompiler);

namespace
{
	shaderc_shader_kind ConvertShaderStage(EVulkanShaderStage Stage)
	{
		switch (Stage)
		{
		case EVulkanShaderStage::Vertex:
			return shaderc_glsl_vertex_shader;
		case EVulkanShaderStage::Fragment:
			return shaderc_glsl_fragment_shader;
		case EVulkanShaderStage::Compute:
			return shaderc_glsl_compute_shader;
		case EVulkanShaderStage::Geometry:
			return shaderc_glsl_geometry_shader;
		case EVulkanShaderStage::Mesh:
			return shaderc_glsl_mesh_shader;
		default:
			VERIFY(false, "Invalid shader stage: {}", static_cast<uint8>(Stage));
			return shaderc_glsl_vertex_shader;
		}
	}
}

FVulkanShaderCompiler::FVulkanShaderCompiler() = default;

FVulkanShaderCompiler::~FVulkanShaderCompiler() = default;

TVector<uint32> FVulkanShaderCompiler::CompileShader(const FVulkanShaderCompilationDesc& Description) const
{
	const FFile File(Description.SourceFile);
	VERIFY(File.Exists(), "Shader file does not exist: {}", Description.SourceFile);
	const FAnsiString SourceContent = File.ReadAnsi();
	const TVector<char> Assembly = CompileToAssembly(PreprocessShader(SourceContent, Description), Description);
	return AssembleToSpirv(Assembly, Description);
}

TVector<char> FVulkanShaderCompiler::PreprocessShader(const FStringView& SourceContent, const FVulkanShaderCompilationDesc& Description) const
{
	const shaderc::PreprocessedSourceCompilationResult PreprocessResult = Compiler.PreprocessGlsl(SourceContent.data(), SourceContent.size(),
	                                                                                              ConvertShaderStage(Description.Stage),
	                                                                                              Description.SourceFile.c_str(), Description.Options);
	if (PreprocessResult.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		RW_LOG(LogShaderCompiler, Error, "Failed to preprocess shader: {}", PreprocessResult.GetErrorMessage());
		ASSERT(false, "Failed to preprocess shader: {}", PreprocessResult.GetErrorMessage());
	}
	return TVector<char>(PreprocessResult.cbegin(), PreprocessResult.cend());
}

TVector<char> FVulkanShaderCompiler::CompileToAssembly(const TVector<char>& PreprocessedSource, const FVulkanShaderCompilationDesc& Description) const
{
	const shaderc::AssemblyCompilationResult AssemblyResult = Compiler.CompileGlslToSpvAssembly(PreprocessedSource.data(), PreprocessedSource.size(),
	                                                                                            ConvertShaderStage(Description.Stage),
	                                                                                            Description.SourceFile.c_str(), Description.Options);
	if (AssemblyResult.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		RW_LOG(LogShaderCompiler, Error, "Failed to compile shader to spirv assembly: {}", AssemblyResult.GetErrorMessage());
		ASSERT(false, "Failed to compile shader to spirv assembly: {}", AssemblyResult.GetErrorMessage());
	}
	return TVector<char>(AssemblyResult.cbegin(), AssemblyResult.cend());
}

TVector<uint32> FVulkanShaderCompiler::AssembleToSpirv(const TVector<char>& Assembly, const FVulkanShaderCompilationDesc& Description) const
{
	const shaderc::SpvCompilationResult SpirvResult = Compiler.AssembleToSpv(Assembly.data(), Assembly.size(), Description.Options);
	if (SpirvResult.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		RW_LOG(LogShaderCompiler, Error, "Failed to assemble spirv: {}", SpirvResult.GetErrorMessage());
		ASSERT(false, "Failed to assemble spirv: {}", SpirvResult.GetErrorMessage());
	}
	return TVector<uint32>(SpirvResult.cbegin(), SpirvResult.cend());
}
