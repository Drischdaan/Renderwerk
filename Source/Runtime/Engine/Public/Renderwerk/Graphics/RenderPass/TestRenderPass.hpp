﻿#pragma once

#include "glm/glm.hpp"

#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"
#include "Renderwerk/Graphics/RenderPass/GfxRenderPass.hpp"

class FEntity;

struct ENGINE_API FGfxCameraConstantBuffer
{
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
};

struct ENGINE_API FGfxModelConstantBuffer
{
	glm::mat4 ModelMatrix;
};

class ENGINE_API FTestRenderPass : public IGfxRenderPass
{
public:
	explicit FTestRenderPass(FGfxDevice* InGfxDevice);
	~FTestRenderPass() override;

	NON_COPY_MOVEABLE(FTestRenderPass)

public:
	void CreateResources(const TRef<FGfxResourceManager>& ResourceManager) override;
	void ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, uint32 NewWidth, uint32 NewHeight) override;
	void ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager) override;

	void Render(const TRef<FGfxCommandList>& CommandList, const TRef<FGfxTexture>& BackBuffer) override;

private:
	void CreateRenderTarget(const TRef<FGfxResourceManager>& ResourceManager, uint32 Width, uint32 Height);
	void CreateDepthStencil(const TRef<FGfxResourceManager>& ResourceManager, uint32 Width, uint32 Height);

	void OnEntityDestroy(FEntity& Entity) const;

	void OnImGuiRender() const;

private:
	TRef<FGfxGraphicsPipeline> GraphicsPipeline;
	TRef<FGfxTexture> RenderTarget;
	TRef<FGfxTexture> DepthStencil;

	TRef<FGfxBuffer> CameraConstantBuffer;
	FGfxCameraConstantBuffer CameraConstantBufferValues = {};

	FDelegateHandle EntityDestroyDelegateHandle;
	FDelegateHandle ImGuiDelegateHandle;
};
