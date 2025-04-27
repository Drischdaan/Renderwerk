#pragma once

#include "Renderwerk/Core/Containers/Vector.hpp"
#include "Renderwerk/Core/Delegates/MulticastDelegate.hpp"
#include "Renderwerk/Graphics/RenderPass/GfxRenderPass.hpp"

class ENGINE_API FGfxImguiRenderPass : public IGfxRenderPass
{
private:
	struct FGfxImguiData
	{
		FGfxDevice* GfxDevice;
		TVector<FGfxDescriptorHandle> DescriptorHandles;
	};

public:
	explicit FGfxImguiRenderPass(FGfxDevice* InGfxDevice);
	~FGfxImguiRenderPass() override;

	NON_COPY_MOVEABLE(FGfxImguiRenderPass)

public:
	void CreateResources(const TRef<FGfxResourceManager>& ResourceManager) override;
	void ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, uint32 NewWidth, uint32 NewHeight) override;
	void ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager) override;
	void Render(const TRef<FGfxCommandList>& CommandList, const TRef<FGfxTexture>& BackBuffer) override;

public:
	[[nodiscard]] static TMulticastDelegate<>& GetImguiRenderDelegate() { return OnImguiRenderDelegate; }

private:
	static TMulticastDelegate<> OnImguiRenderDelegate;

private:
	FGfxImguiData* ImguiData = nullptr;
};
