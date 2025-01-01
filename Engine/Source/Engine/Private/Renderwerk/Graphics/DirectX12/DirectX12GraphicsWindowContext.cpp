#include "pch.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsWindowContext.h"

FDirectX12GraphicsWindowContext::FDirectX12GraphicsWindowContext(IGraphicsBackend* InBackend)
	: IGraphicsWindowContext(InBackend)
{
}

FDirectX12GraphicsWindowContext::~FDirectX12GraphicsWindowContext() = default;

void FDirectX12GraphicsWindowContext::Initialize(const TSharedPtr<FWindow>& InWindow)
{
	Window = InWindow;
}

void FDirectX12GraphicsWindowContext::Destroy()
{
	Window.reset();
}
