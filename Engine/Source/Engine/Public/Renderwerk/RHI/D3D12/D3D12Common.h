#pragma once
// ReSharper disable CppUnusedIncludeDirective

#include "Renderwerk/RHI/RHICommon.h"
#include "Renderwerk/RHI/D3D12/D3D12Fwd.h"

#include "d3d12.h"
#include "d3d12sdklayers.h"
#include "d3dx12/d3dx12.h"

#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>

#pragma region Definitions

#define D3D_CHECK(ResultPredicate) \
	{ \
		FD3D12Result UNIQUE_NAME(Result) = ResultPredicate; \
		ASSERTR(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result)) \
	}

#define D3D_CHECKM(ResultPredicate, ...) \
	{ \
		FD3D12Result UNIQUE_NAME(Result) = ResultPredicate; \
		ASSERTRM(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result), __VA_ARGS__) \
	}

#if RW_CONFIG_DEBUG || RW_CONFIG_DEVELOPMENT
#	define DEBUG_D3D_CHECK(ResultPredicate) D3D_CHECK(ResultPredicate)
#	define DEBUG_D3D_CHECKM(ResultPredicate, ...) D3D_CHECKM(ResultPredicate, __VA_ARGS__)
#else
#	define DEBUG_D3D_CHECK(ResultPredicate) { ResultPredicate; }
#	define DEBUG_D3D_CHECKM(ResultPredicate, ...) { ResultPredicate; }
#endif

#pragma endregion

DECLARE_LOG_CATEGORY(LogD3D12, Trace);

template <typename T>
using TComPtr = Microsoft::WRL::ComPtr<T>;

using FD3D12Result = const HRESULT;

#pragma region ToString Functions

RENDERWERK_API FString ToString(D3D_FEATURE_LEVEL FeatureLevel);
RENDERWERK_API FString ToString(D3D_SHADER_MODEL ShaderModel);

#pragma endregion
