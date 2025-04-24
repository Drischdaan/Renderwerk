#include "pch.hpp"

#include "Renderwerk/Renderer/ShaderCompiler.hpp"

constexpr const FWideChar* GetTargetProfile(const EShaderType Type)
{
	switch (Type)
	{
	case EShaderType::Vertex: return L"vs_6_7";
	case EShaderType::Pixel: return L"ps_6_7";
	case EShaderType::Compute: return L"cs_6_7";
	case EShaderType::Hull: return L"hs_6_7";
	case EShaderType::Domain: return L"ds_6_7";
	case EShaderType::Mesh: return L"ms_6_7";
	case EShaderType::Amplification: return L"as_6_7";
	case EShaderType::Library: return L"lib_6_6";
	case EShaderType::None:
		break;
	}
	return L"";
}

FShaderCompiler::FShaderCompiler()
{
	HRESULT Result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&DxcUtils));
	RW_VERIFY_ID(Result);

	Result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DxcCompiler));
	RW_VERIFY_ID(Result);
}

FShaderCompiler::~FShaderCompiler()
{
	DxcCompiler.Reset();
	DxcUtils.Reset();
}

FShaderModule FShaderCompiler::CompilerShader(const EShaderType Type, const FAnsiString& ShaderSource, const FWideString& Entrypoint) const
{
	TObjectHandle<IDxcIncludeHandler> IncludeHandler;
	HRESULT Result = DxcUtils->CreateDefaultIncludeHandler(&IncludeHandler);
	RW_VERIFY_ID(Result);

	TObjectHandle<IDxcBlobEncoding> SourceBlob = nullptr;
	Result = DxcUtils->CreateBlob(ShaderSource.data(), static_cast<uint32>(ShaderSource.size()), 0, &SourceBlob);
	RW_VERIFY_ID(Result);

	TVector<const FWideChar*> Arguments = {
		L"-E",
		Entrypoint.c_str(),
		L"-T",
		GetTargetProfile(Type),
		L"-Qstrip_debug",
		L"-Qstrip_reflect",
		DXC_ARG_WARNINGS_ARE_ERRORS,
		DXC_ARG_DEBUG,
	};

	DxcBuffer SourceBuffer;
	SourceBuffer.Ptr = SourceBlob->GetBufferPointer();
	SourceBuffer.Size = SourceBlob->GetBufferSize();
	SourceBuffer.Encoding = 0;

	TObjectHandle<IDxcResult> CompileResult = nullptr;
	Result = DxcCompiler->Compile(&SourceBuffer, Arguments.data(), static_cast<uint32>(Arguments.size()), IncludeHandler, IID_PPV_ARGS(&CompileResult));
	RW_VERIFY_ID(Result);

	TObjectHandle<IDxcBlobUtf8> ErrorBlob;
	Result = CompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&ErrorBlob), nullptr);
	RW_VERIFY_ID(Result);
	if (ErrorBlob && ErrorBlob->GetStringLength() > 0)
	{
		printf("%s\n", ErrorBlob->GetStringPointer());
		RW_VERIFY_MSG(false, "Unable to compiler shader");
	}

	TObjectHandle<IDxcBlob> ShaderBytecode;
	Result = CompileResult->GetResult(&ShaderBytecode);
	RW_VERIFY_ID(Result);

	TObjectHandle<IDxcBlob> ReflectionData;
	Result = CompileResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&ReflectionData), nullptr);
	RW_VERIFY_ID(Result);

	FShaderModule ShaderModule = {};
	ShaderModule.Type = Type;
	ShaderModule.ByteCode.resize(ShaderBytecode->GetBufferSize());
	FMemory::Copy(ShaderModule.ByteCode.data(), ShaderBytecode->GetBufferPointer(), ShaderBytecode->GetBufferSize());
	ShaderModule.ReflectionByteCode.resize(ReflectionData->GetBufferSize());
	FMemory::Copy(ShaderModule.ReflectionByteCode.data(), ReflectionData->GetBufferPointer(), ReflectionData->GetBufferSize());
	return ShaderModule;
}

TObjectHandle<ID3D12ShaderReflection> FShaderCompiler::CreateShaderReflection(const FShaderModule& ShaderModule) const
{
	DxcBuffer ShaderBuffer = {};
	ShaderBuffer.Ptr = ShaderModule.ReflectionByteCode.data();
	ShaderBuffer.Size = ShaderModule.ReflectionByteCode.size();

	TObjectHandle<ID3D12ShaderReflection> Reflection;
	const HRESULT Result = DxcUtils->CreateReflection(&ShaderBuffer, IID_PPV_ARGS(&Reflection));
	RW_VERIFY_ID(Result);
	return Reflection;
}
