#include "pch.h"

#include "Renderwerk/RHI/RHI.h"

#include "Renderwerk/RHI/D3D12/D3D12RHI.h"

IRHI::IRHI(const FRHIDesc& InDescription)
	: Description(InDescription)
{
}

TLocalPtr<IRHI> IRHI::Create(const ERHIType Type, const FRHIDesc& Description)
{
	switch (Type)
	{
	case ERHIType::D3D12: return MakeLocal<FD3D12RHI>(Description);
	case ERHIType::None:
	default:
		return nullptr;
	}
}
