#pragma once

#include "Buffer.hpp"
#include "Texture.hpp"

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Renderer/GraphicsCommon.hpp"

class FDevice;

class ENGINE_API FCommandList : public TGraphicsObject<FCommandList>
{
public:
	FCommandList(FDevice* InDevice, D3D12_COMMAND_LIST_TYPE InType);
	~FCommandList() override;

	NON_COPY_MOVEABLE(FCommandList)

public:
	void Open() const;
	void Close() const;

	void ClearRenderTarget(const TObjectHandle<FTexture>& Texture, const float32 Colors[4]) const;
	void SetRenderTarget(const TObjectHandle<FTexture>& Texture) const;

	void ResourceBarrier(const TObjectHandle<IResource>& Resource, D3D12_RESOURCE_STATES NewState) const;
	void ResourceBarrier(const TObjectHandle<FTexture>& Texture, D3D12_RESOURCE_STATES NewState) const;
	void ResourceBarrier(const TObjectHandle<FBuffer>& Buffer, D3D12_RESOURCE_STATES NewState) const;

	void CopyBufferToTexture(const TObjectHandle<IResource>& Destination, const TObjectHandle<FBuffer>& Source) const;

public:
	[[nodiscard]] TObjectHandle<ID3D12GraphicsCommandList10> GetHandle() const { return CommandList; }

private:
	FDevice* Device;
	D3D12_COMMAND_LIST_TYPE Type;

	TObjectHandle<ID3D12CommandAllocator> CommandAllocator;
	TObjectHandle<ID3D12GraphicsCommandList10> CommandList;
};
