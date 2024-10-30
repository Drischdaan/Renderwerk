#pragma once

#include "Renderwerk/RHI/RHICommon.h"

struct RENDERWERK_API FRHIDesc
{
	bool8 bEnableDebugLayer = false;
	bool8 bEnableValidationLayer = false;
};

class RENDERWERK_API IRHI
{
public:
	IRHI(const FRHIDesc& InDescription);
	virtual ~IRHI() = default;

	DELETE_COPY_AND_MOVE(IRHI);

public:
	NODISCARD virtual TVector<TSharedPtr<IAdapter>> QueryAdapters() = 0;

public:
	NODISCARD static TLocalPtr<IRHI> Create(ERHIType Type, const FRHIDesc& Description);

protected:
	FRHIDesc Description;
};
