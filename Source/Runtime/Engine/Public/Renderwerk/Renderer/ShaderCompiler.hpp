#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

#include "d3d12shader.h"
#include "dxcapi.h"

enum class ENGINE_API EShaderType : uint8
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

struct ENGINE_API FShaderModule
{
	EShaderType Type;
	TVector<uint8> ByteCode;
	TVector<uint8> ReflectionByteCode;
};

class ENGINE_API FShaderCompiler : public TGraphicsObject<FShaderCompiler>
{
public:
	FShaderCompiler();
	~FShaderCompiler() override;

	NON_COPY_MOVEABLE(FShaderCompiler)

public:
	[[nodiscard]] FShaderModule CompilerShader(EShaderType Type, const FAnsiString& ShaderSource, const FWideString& Entrypoint) const;
	[[nodiscard]] TObjectHandle<ID3D12ShaderReflection> CreateShaderReflection(const FShaderModule& ShaderModule) const;

private:
	TObjectHandle<IDxcUtils> DxcUtils;
	TObjectHandle<IDxcCompiler3> DxcCompiler;
};
