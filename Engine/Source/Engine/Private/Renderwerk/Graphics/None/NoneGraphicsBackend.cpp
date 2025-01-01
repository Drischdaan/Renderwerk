#include "pch.h"

#include "Renderwerk/Graphics/None/NoneGraphicsBackend.h"

FNoneGraphicsBackend::FNoneGraphicsBackend()
	: IGraphicsBackend(EGraphicsBackendType::None)
{
}

FNoneGraphicsBackend::~FNoneGraphicsBackend() = default;

void FNoneGraphicsBackend::Initialize(const FGraphicsBackendDesc& InDescription)
{
	Description = InDescription;
}

void FNoneGraphicsBackend::Destroy()
{
}

TVector<TSharedPtr<IGraphicsAdapter>> FNoneGraphicsBackend::GetAvailableAdapters()
{
	return {};
}

bool8 FNoneGraphicsBackend::IsAdapterSuitable(const TSharedPtr<IGraphicsAdapter>& GraphicsAdapter)
{
	return false;
}

TSharedPtr<IGraphicsWindowContext> FNoneGraphicsBackend::CreateWindowContext()
{
	return nullptr;
}
