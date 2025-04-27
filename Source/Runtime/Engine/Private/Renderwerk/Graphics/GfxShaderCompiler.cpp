#include "pch.hpp"

#include "Renderwerk/Graphics/GfxShaderCompiler.hpp"

#include "Renderwerk/Core/Memory/Memory.hpp"

FGfxShaderCompiler::FGfxShaderCompiler(FGfxDevice* InGfxDevice, const D3D_SHADER_MODEL InShaderModel)
	: FGfxShaderCompiler(InGfxDevice, InShaderModel, TEXT("ShaderCompiler"))
{
}

FGfxShaderCompiler::FGfxShaderCompiler(FGfxDevice* InGfxDevice, const D3D_SHADER_MODEL InShaderModel, const FStringView& InDebugName)
	: IGfxDeviceChild(InGfxDevice, InDebugName), ShaderModel(InShaderModel)
{
	HRESULT Result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&DxcUtils));
	RW_VERIFY_ID(Result);

	Result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DxcCompiler));
	RW_VERIFY_ID(Result);
}

FGfxShaderCompiler::~FGfxShaderCompiler()
{
	DxcCompiler.Reset();
	DxcUtils.Reset();
}

FGfxShaderModule FGfxShaderCompiler::CompilerShader(const EGfxShaderType Type, const FAnsiString& ShaderSource, const FWideString& Entrypoint) const
{
	TComPtr<IDxcIncludeHandler> IncludeHandler;
	HRESULT Result = DxcUtils->CreateDefaultIncludeHandler(&IncludeHandler);
	RW_VERIFY_ID(Result);

	TComPtr<IDxcBlobEncoding> SourceBlob = nullptr;
	Result = DxcUtils->CreateBlob(ShaderSource.data(), static_cast<uint32>(ShaderSource.size()), 0, &SourceBlob);
	RW_VERIFY_ID(Result);

	const FString TargetProfile = GetTargetProfile(Type);
	TVector<const FWideChar*> Arguments = {
		L"-E",
		Entrypoint.c_str(),
		L"-T",
		TargetProfile.c_str(),
		L"-Qstrip_debug",
		L"-Qstrip_reflect",
		DXC_ARG_WARNINGS_ARE_ERRORS,
		DXC_ARG_DEBUG,
	};

	DxcBuffer SourceBuffer;
	SourceBuffer.Ptr = SourceBlob->GetBufferPointer();
	SourceBuffer.Size = SourceBlob->GetBufferSize();
	SourceBuffer.Encoding = 0;

	TComPtr<IDxcResult> CompileResult = nullptr;
	Result = DxcCompiler->Compile(&SourceBuffer, Arguments.data(), static_cast<uint32>(Arguments.size()), IncludeHandler.Get(), IID_PPV_ARGS(&CompileResult));
	RW_VERIFY_ID(Result);

	TComPtr<IDxcBlobUtf8> ErrorBlob;
	Result = CompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&ErrorBlob), nullptr);
	RW_VERIFY_ID(Result);
	if (ErrorBlob && ErrorBlob->GetStringLength() > 0)
	{
		printf("%s\n", ErrorBlob->GetStringPointer());
		RW_VERIFY_MSG(false, "Unable to compiler shader");
	}

	TComPtr<IDxcBlob> ShaderBytecode;
	Result = CompileResult->GetResult(&ShaderBytecode);
	RW_VERIFY_ID(Result);

	TComPtr<IDxcBlob> ReflectionData;
	Result = CompileResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&ReflectionData), nullptr);
	RW_VERIFY_ID(Result);

	FGfxShaderModule ShaderModule = {};
	ShaderModule.Type = Type;
	ShaderModule.ByteCode.resize(ShaderBytecode->GetBufferSize());
	FMemory::Copy(ShaderModule.ByteCode.data(), ShaderBytecode->GetBufferPointer(), ShaderBytecode->GetBufferSize());
	ShaderModule.ReflectionByteCode.resize(ReflectionData->GetBufferSize());
	FMemory::Copy(ShaderModule.ReflectionByteCode.data(), ReflectionData->GetBufferPointer(), ReflectionData->GetBufferSize());
	return ShaderModule;
}

TComPtr<ID3D12ShaderReflection> FGfxShaderCompiler::CreateShaderReflection(const FGfxShaderModule& ShaderModule) const
{
	DxcBuffer ShaderBuffer = {};
	ShaderBuffer.Ptr = ShaderModule.ReflectionByteCode.data();
	ShaderBuffer.Size = ShaderModule.ReflectionByteCode.size();

	TComPtr<ID3D12ShaderReflection> Reflection;
	const HRESULT Result = DxcUtils->CreateReflection(&ShaderBuffer, IID_PPV_ARGS(&Reflection));
	RW_VERIFY_ID(Result);
	return Reflection;
}

FStringView FGfxShaderCompiler::GetShaderModelString() const
{
	switch (ShaderModel)
	{
	case D3D_SHADER_MODEL_5_1: return TEXT("5_1");
	case D3D_SHADER_MODEL_6_0: return TEXT("6_0");
	case D3D_SHADER_MODEL_6_1: return TEXT("6_1");
	case D3D_SHADER_MODEL_6_2: return TEXT("6_2");
	case D3D_SHADER_MODEL_6_3: return TEXT("6_3");
	case D3D_SHADER_MODEL_6_4: return TEXT("6_4");
	case D3D_SHADER_MODEL_6_5: return TEXT("6_5");
	case D3D_SHADER_MODEL_6_6: return TEXT("6_6");
	case D3D_SHADER_MODEL_6_7: return TEXT("6_7");
	case D3D_SHADER_MODEL_6_8: return TEXT("6_8");
	case D3D_SHADER_MODEL_6_9: return TEXT("6_9");
	case D3D_SHADER_MODEL_NONE: return TEXT("none");
	}
	RW_VERIFY_MSG(false, "Unknown shader model");
	return TEXT("");
}

FString FGfxShaderCompiler::GetTargetProfile(const EGfxShaderType Type) const
{
	FString Target;
	switch (Type)
	{
	case EGfxShaderType::None:
		Target += TEXT("none");
		break;
	case EGfxShaderType::Vertex:
		Target += TEXT("vs_");
		break;
	case EGfxShaderType::Pixel:
		Target += TEXT("ps_");
		break;
	case EGfxShaderType::Compute:
		Target += TEXT("cs_");
		break;
	case EGfxShaderType::Hull:
		Target += TEXT("hs_");
		break;
	case EGfxShaderType::Domain:
		Target += TEXT("ds_");
		break;
	case EGfxShaderType::Mesh:
		Target += TEXT("ms_");
		break;
	case EGfxShaderType::Amplification:
		Target += TEXT("as_");
		break;
	case EGfxShaderType::Library:
		Target += TEXT("lib_");
		break;
	}
	return Target + GetShaderModelString().data();
}
