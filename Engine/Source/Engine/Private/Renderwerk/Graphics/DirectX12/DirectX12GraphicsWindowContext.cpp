#include "pch.h"

#include "Renderwerk/Graphics/DirectX12/DirectX12GraphicsWindowContext.h"

#include "Renderwerk/Platform/Window.h"

FDirectX12GraphicsWindowContext::FDirectX12GraphicsWindowContext(IGraphicsBackend* InBackend)
	: IGraphicsWindowContext(InBackend)
{
}

FDirectX12GraphicsWindowContext::~FDirectX12GraphicsWindowContext() = default;

void FDirectX12GraphicsWindowContext::Initialize(const TSharedPtr<FWindow>& InWindow)
{
	Window = InWindow;

	RW_LOG(LogGraphics, Debug, "Created directx12 window context for window '{}' ('{}')", Window->GetId(), Window->GetState().Title);
}

void FDirectX12GraphicsWindowContext::Destroy()
{
	Window.reset();
}
