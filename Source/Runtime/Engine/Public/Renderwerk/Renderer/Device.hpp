#pragma once

#include "ShaderCompiler.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/CommandList.hpp"
#include "Renderwerk/Renderer/DescriptorHeap.hpp"
#include "Renderwerk/Renderer/Fence.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"
#include "Renderwerk/Renderer/ResourceUploader.hpp"
#include "Renderwerk/Renderer/Texture.hpp"

class FAdapter;

struct ENGINE_API FDeviceDesc
{
	uint32 MaxRenderTargets = 100;
	uint32 MaxSamplers = 100;
	uint32 MaxDepthStencils = 100;
	uint32 MaxShaderResources = 100;
	bool bEnableInfoQueue = false;
};

class ENGINE_API FDevice : public TGraphicsObject<FDevice>
{
public:
	FDevice(FAdapter* InAdapter, const FDeviceDesc& InDeviceDesc);
	~FDevice() override;

	NON_COPY_MOVEABLE(FDevice)

public:
	void FlushWork() const;

	void SubmitGraphicsWork(const TObjectHandle<FCommandList>& CommandList) const;
	void SignalGraphicsQueue(const TObjectHandle<FFence>& Fence) const;

	[[nodiscard]] TObjectHandle<FFence> CreateFence();
	[[nodiscard]] TObjectHandle<FTexture> CreateTexture(const FTextureDesc& TextureDesc);

	void UploadBuffer(const TObjectHandle<FBuffer>& Buffer, const void* Data, size64 DataSize) const;
	void UploadTexture(const TObjectHandle<FTexture>& Texture, const TVector<uint8>& Data) const;

public:
	[[nodiscard]] TObjectHandle<ID3D12Device14> GetHandle() const { return Device; }

	[[nodiscard]] FAdapter* GetAdapter() const { return Adapter; }

	[[nodiscard]] TObjectHandle<ID3D12CommandQueue> GetGraphicsCommandQueue() const { return GraphicsCommandQueue; }
	[[nodiscard]] TObjectHandle<ID3D12CommandQueue> GetComputeCommandQueue() const { return ComputeCommandQueue; }
	[[nodiscard]] TObjectHandle<ID3D12CommandQueue> GetCopyCommandQueue() const { return CopyCommandQueue; }

	[[nodiscard]] TObjectHandle<FDescriptorHeap> GetRTVDescriptorHeap() const { return RTVDescriptorHeap; }
	[[nodiscard]] TObjectHandle<FDescriptorHeap> GetSamplerDescriptorHeap() const { return SamplerDescriptorHeap; }
	[[nodiscard]] TObjectHandle<FDescriptorHeap> GetDSVDescriptorHeap() const { return DSVDescriptorHeap; }
	[[nodiscard]] TObjectHandle<FDescriptorHeap> GetRSVDescriptorHeap() const { return RSVDescriptorHeap; }

	[[nodiscard]] TObjectHandle<FResourceUploader> GetResourceUploader() const { return ResourceUploader; }
	[[nodiscard]] TObjectHandle<FShaderCompiler> GetShaderCompiler() const { return ShaderCompiler; }

private:
	TObjectHandle<ID3D12CommandQueue> CreateInternalCommandQueue(D3D12_COMMAND_LIST_TYPE Type) const;

private:
	FDeviceDesc DeviceDesc;

	FAdapter* Adapter;

	TObjectHandle<ID3D12Device14> Device;

	TObjectHandle<ID3D12InfoQueue1> InfoQueue;
	DWORD InfoQueueCookie = 0;

	TObjectHandle<FFence> WorkFence;
	TObjectHandle<ID3D12CommandQueue> GraphicsCommandQueue;
	TObjectHandle<ID3D12CommandQueue> ComputeCommandQueue;
	TObjectHandle<ID3D12CommandQueue> CopyCommandQueue;

	TObjectHandle<FDescriptorHeap> RTVDescriptorHeap;
	TObjectHandle<FDescriptorHeap> SamplerDescriptorHeap;
	TObjectHandle<FDescriptorHeap> DSVDescriptorHeap;
	TObjectHandle<FDescriptorHeap> RSVDescriptorHeap;

	TObjectHandle<FResourceUploader> ResourceUploader;
	TObjectHandle<FShaderCompiler> ShaderCompiler;
};
