#include "pch.hpp"

#include "Renderwerk/Graphics/RenderPass/GfxImguiRenderPass.hpp"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDescriptorHeap.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxSurface.hpp"
#include "Renderwerk/Platform/Window.hpp"

TMulticastDelegate<> FGfxImguiRenderPass::OnImguiRenderDelegate;

FGfxImguiRenderPass::FGfxImguiRenderPass(FGfxDevice* InGfxDevice)
	: IGfxRenderPass(InGfxDevice, TEXT("ImguiRenderPass"))
{
}

FGfxImguiRenderPass::~FGfxImguiRenderPass() = default;

void FGfxImguiRenderPass::CreateResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& Io = ImGui::GetIO();
	Io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	Io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetEngine()->GetWindow()->GetHandle());

	ImGui_ImplDX12_InitInfo InitInfo = {};
	InitInfo.Device = GfxDevice->GetNativeObject<ID3D12Device>(NativeObjectIds::D3D12_Device);
	InitInfo.CommandQueue = GfxDevice->GetGraphicsQueue().Get();
	InitInfo.NumFramesInFlight = RW_DEFAULT_BUFFER_COUNT;
	InitInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	InitInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
	ImguiData = FMemory::NewInstance<FGfxImguiData>();
	ImguiData->GfxDevice = GfxDevice;
	InitInfo.UserData = ImguiData;

	InitInfo.SrvDescriptorHeap = GfxDevice->GetSRVDescriptorHeap()->GetNativeObject<ID3D12DescriptorHeap>(NativeObjectIds::D3D12_DescriptorHeap);
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	InitInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* InitInfo, D3D12_CPU_DESCRIPTOR_HANDLE* OutCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE* OutGPUHandle)
	{
		FGfxImguiData* ImguiData = static_cast<FGfxImguiData*>(InitInfo->UserData);
		const FGfxDescriptorHandle Handle = ImguiData->GfxDevice->GetSRVDescriptorHeap()->Allocate();
		ImguiData->DescriptorHandles.push_back(Handle);
		*OutCPUHandle = Handle.GetCPUHandle();
		*OutGPUHandle = Handle.GetGPUHandle();
	};
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	InitInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* InitInfo, const D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle)
	{
		FGfxImguiData* ImguiData = static_cast<FGfxImguiData*>(InitInfo->UserData);
		FGfxDescriptorHandle Handle;
		for (FGfxDescriptorHandle& TempHandle : ImguiData->DescriptorHandles)
		{
			if (TempHandle.GetCPUHandle().ptr == CPUHandle.ptr && TempHandle.GetGPUHandle().ptr == GPUHandle.ptr)
			{
				Handle = TempHandle;
			}
		}
		RW_VERIFY(Handle.IsValid());
		ImguiData->GfxDevice->GetSRVDescriptorHeap()->Free(Handle);
	};
	ImGui_ImplDX12_Init(&InitInfo);
}

void FGfxImguiRenderPass::ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, const uint32 NewWidth, const uint32 NewHeight)
{
	IGfxRenderPass::ResizeResources(ResourceManager, NewWidth, NewHeight);
}

void FGfxImguiRenderPass::ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	FMemory::DestroyInstance(ImguiData);
}

void FGfxImguiRenderPass::Render(const TRef<FGfxCommandList>& CommandList, const TRef<FGfxTexture>& BackBuffer)
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		OnImguiRenderDelegate.Broadcast();
	}
	ImGui::Render();

	ID3D12GraphicsCommandList10* NativeCommandList = CommandList->GetNativeObject<ID3D12GraphicsCommandList10>(NativeObjectIds::D3D12_CommandList);
	ID3D12DescriptorHeap* NativeDescriptorHeap = GfxDevice->GetSRVDescriptorHeap()->GetNativeObject<ID3D12DescriptorHeap>(NativeObjectIds::D3D12_DescriptorHeap);
	NativeCommandList->SetDescriptorHeaps(1, &NativeDescriptorHeap);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList->GetNativeObject<ID3D12GraphicsCommandList>(NativeObjectIds::D3D12_CommandList));
}
