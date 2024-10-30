#include "pch.h"

#include "Renderwerk/RHI/D3D12/D3D12Common.h"

FString ToString(const D3D_FEATURE_LEVEL FeatureLevel)
{
	switch (FeatureLevel)
	{
	case D3D_FEATURE_LEVEL_1_0_GENERIC: return TEXT("1.0 (Generic)");
	case D3D_FEATURE_LEVEL_1_0_CORE: return TEXT("1.0 (Core)");
	case D3D_FEATURE_LEVEL_9_1: return TEXT("9.1");
	case D3D_FEATURE_LEVEL_9_2: return TEXT("9.2");
	case D3D_FEATURE_LEVEL_9_3: return TEXT("9.3");
	case D3D_FEATURE_LEVEL_10_0: return TEXT("10.0");
	case D3D_FEATURE_LEVEL_10_1: return TEXT("10.1");
	case D3D_FEATURE_LEVEL_11_0: return TEXT("11.0");
	case D3D_FEATURE_LEVEL_11_1: return TEXT("11.1");
	case D3D_FEATURE_LEVEL_12_0: return TEXT("12.0");
	case D3D_FEATURE_LEVEL_12_1: return TEXT("12.1");
	case D3D_FEATURE_LEVEL_12_2: return TEXT("12.2");
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const D3D_SHADER_MODEL ShaderModel)
{
	switch (ShaderModel)
	{
	case D3D_SHADER_MODEL_NONE: return TEXT("None");
	case D3D_SHADER_MODEL_5_1: return TEXT("5.1");
	case D3D_SHADER_MODEL_6_0: return TEXT("6.0");
	case D3D_SHADER_MODEL_6_1: return TEXT("6.1");
	case D3D_SHADER_MODEL_6_2: return TEXT("6.2");
	case D3D_SHADER_MODEL_6_3: return TEXT("6.3");
	case D3D_SHADER_MODEL_6_4: return TEXT("6.4");
	case D3D_SHADER_MODEL_6_5: return TEXT("6.5");
	case D3D_SHADER_MODEL_6_6: return TEXT("6.6");
	case D3D_SHADER_MODEL_6_7: return TEXT("6.7");
	case D3D_SHADER_MODEL_6_8: return TEXT("6.8");
	case D3D_SHADER_MODEL_6_9: return TEXT("6.9");
	default:
		return TEXT("Unknown");
	}
}
