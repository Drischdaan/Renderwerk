#include "pch.hpp"

#include "Renderwerk/Renderer/RendererModule.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"
#include "Renderwerk/Renderer/ResourceUploader.hpp"

TVector<uint8_t> CreateCheckerboardImage()
{
	constexpr int width = 16;
	constexpr int height = 16;
	TVector<uint8_t> imageData(width * height);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			const bool isDark = (x / 4 + y / 4) % 2 == 0;
			imageData[y * width + x] = isDark ? 50 : 200;
		}
	}
	return imageData;
}

FRendererModule::FRendererModule()
	: IEngineModule(EEngineThreadAffinity::Render)
{
}

void FRendererModule::Initialize()
{
	PROFILE_FUNCTION();

	FGraphicsContextDesc ContextDesc = {
#ifdef RW_CONFIG_DEBUG
		.bEnableDebugLayer = true,
		.bEnableGPUValidation = true,
#else
		.bEnableDebugLayer = false,
		.bEnableGPUValidation = false,
#endif
	};
	Context = NewObjectHandle<FGraphicsContext>(ContextDesc);

	Adapter = Context->GetSuitableAdapter();
	RW_VERIFY_MSG(Adapter, "Unable to find suitable adapter");
	RW_LOG(Info, "Using adapter:");
	RW_LOG(Info, "\t- Name: {}", Adapter->GetName());
	RW_LOG(Info, "\t- Vendor: {}", FAdapter::GetVendorString(Adapter->GetVendor()));
	RW_LOG(Info, "\t- VideoMemory: {}", FStringUtilities::FormatMemorySize(Adapter->GetDedicatedVideoMemory()));
	RW_LOG(Info, "\t- SystemMemory: {}", FStringUtilities::FormatMemorySize(Adapter->GetSharedSystemMemory()));
	RW_LOG(Info, "\t- FeatureLevel: {}", D3DUtility::ToString(Adapter->GetFeatureLevel()));
	RW_LOG(Info, "\t- ShaderModel: {}", D3DUtility::ToString(Adapter->GetShaderModel()));
	RW_LOG(Info, "\t- ResourceBinding: {}", D3DUtility::ToString(Adapter->GetResourceBindingTier()));
	RW_LOG(Info, "\t- Raytracing: {}", D3DUtility::ToString(Adapter->GetRaytracingTier()));
	RW_LOG(Info, "\t- MeshShader: {}", D3DUtility::ToString(Adapter->GetMeshShaderTier()));

	RW_LOG(Info, "Adapter Memory Stats:");
	{
		const FAdapterMemoryStats DedicatedStats = Adapter->QueryMemoryStats(EAdapterMemoryType::Dedicated);
		RW_LOG(Info, "\t- Dedicated:");
		RW_LOG(Info, "\t\t- Available: {}", FStringUtilities::FormatMemorySize(DedicatedStats.AvailableMemory));
		RW_LOG(Info, "\t\t- Usage: {}", FStringUtilities::FormatMemorySize(DedicatedStats.UsedMemory));

		const FAdapterMemoryStats SharedStats = Adapter->QueryMemoryStats(EAdapterMemoryType::Shared);
		RW_LOG(Info, "\t- Shared:");
		RW_LOG(Info, "\t\t- Available: {}", FStringUtilities::FormatMemorySize(SharedStats.AvailableMemory));
		RW_LOG(Info, "\t\t- Usage: {}", FStringUtilities::FormatMemorySize(SharedStats.UsedMemory));
	}

	constexpr FDeviceDesc DeviceDesc = {
#ifdef RW_CONFIG_DEBUG
		.bEnableInfoQueue = true,
#else
		.bEnableInfoQueue = false,
#endif
	};
	Device = Adapter->CreateDevice(DeviceDesc);

	FSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Window = GetEngine()->GetWindow();
	Swapchain = NewObjectHandle<FSwapchain>(Device.Get(), SwapchainDesc);

	Frames.resize(RW_DEFAULT_FRAME_COUNT);
	for (uint32 Index = 0; Index < RW_DEFAULT_FRAME_COUNT; ++Index)
	{
		Frames[Index].CommandList = NewObjectHandle<FCommandList>(Device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
		Frames[Index].Fence = Device->CreateFence();
	}

	UploadCommandList = NewObjectHandle<FCommandList>(Device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Create texture and queue upload
	FTextureDesc TextureDesc = {};
	TextureDesc.Width = 16;
	TextureDesc.Height = 16;
	TextureDesc.Usage = ETextureUsage::RenderTarget;
	TextureDesc.MipLevels = 1;
	TextureDesc.DebugName = TEXT("TestTexture");
	Texture = Device->CreateTexture(TextureDesc);
	Device->UploadTexture(Texture, CreateCheckerboardImage());

	// Upload queued resources
	const TObjectHandle<FResourceUploader> ResourceUploader = Device->GetResourceUploader();
	UploadCommandList->Open();
	ResourceUploader->Flush(UploadCommandList);
	UploadCommandList->Close();
	Device->SubmitGraphicsWork(UploadCommandList);
	Device->FlushWork();
	ResourceUploader->ReleaseResources();

	TickDelegateHandle = GetEngine()->GetRenderThreadTickDelegate().BindRaw(this, &FRendererModule::OnTick);

	RW_LOG(Info, "Renderer module initialized");
}

void FRendererModule::Shutdown()
{
	GetEngine()->GetRenderThreadTickDelegate().Unbind(TickDelegateHandle);

	for (size64 Index = 0; Index < Frames.size(); ++Index)
	{
		Device->FlushWork();
	}

	UploadCommandList.Reset();
	Frames.clear();
	Texture.Reset();
	Swapchain.Reset();

	Device.Reset();
	Adapter.Reset();
	Context.Reset();
	RW_LOG(Info, "Renderer module shutdown");
}

void FRendererModule::OnTick()
{
	const uint32 BackBufferIndex = Swapchain->GetBackBufferIndex();
	const FGraphicsFrame& Frame = Frames.at(FrameIndex);
	RW_VERIFY(Frame.Fence->Wait());

	const TObjectHandle<FCommandList> CommandList = Frame.CommandList;
	{
		PROFILE_SCOPE("CommandListRecording");

		const TObjectHandle<FTexture> BackBuffer = Swapchain->GetBackBuffer(BackBufferIndex);

		CommandList->Open();
		CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
		constexpr float32 Colors[4] = {1.0f, 0.5f, 0.5f, 1.0f};
		CommandList->ClearRenderTarget(BackBuffer, Colors);
		CommandList->SetRenderTarget(BackBuffer);
		CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_PRESENT);
		CommandList->Close();
	}
	Device->SubmitGraphicsWork(CommandList);
	Swapchain->Present(false);
	Device->SignalGraphicsQueue(Frame.Fence);
	FrameIndex = (FrameIndex + 1) % RW_DEFAULT_FRAME_COUNT;
}
