#pragma once

#include "Renderwerk/Graphics/GfxCommon.hpp"

#include "dxcapi.h"

#include "Renderwerk/Core/Containers/Vector.hpp"

enum class ENGINE_API EGfxShaderType : uint8
{
	None = 0,
	Vertex,
	Pixel,
	Compute,
	Hull,
	Domain,
	Mesh,
	Amplification,
	Library,
};

struct ENGINE_API FGfxShaderModule
{
	EGfxShaderType Type;
	TVector<uint8> ByteCode;
	TVector<uint8> ReflectionByteCode;
};

class ENGINE_API FGfxShaderCompiler : public IGfxDeviceChild
{
public:
	explicit FGfxShaderCompiler(FGfxDevice* InGfxDevice, D3D_SHADER_MODEL InShaderModel);
	FGfxShaderCompiler(FGfxDevice* InGfxDevice, D3D_SHADER_MODEL InShaderModel, const FStringView& InDebugName);
	~FGfxShaderCompiler() override;

	NON_COPY_MOVEABLE(FGfxShaderCompiler)

public:
	[[nodiscard]] FGfxShaderModule CompilerShader(EGfxShaderType Type, const FAnsiString& ShaderSource, const FWideString& Entrypoint) const;
	[[nodiscard]] TComPtr<ID3D12ShaderReflection> CreateShaderReflection(const FGfxShaderModule& ShaderModule) const;

private:
	[[nodiscard]] FStringView GetShaderModelString() const;
	[[nodiscard]] FString GetTargetProfile(EGfxShaderType Type) const;

private:
	D3D_SHADER_MODEL ShaderModel;

	TComPtr<IDxcUtils> DxcUtils;
	TComPtr<IDxcCompiler3> DxcCompiler;
};
