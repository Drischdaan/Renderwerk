#include "pch.hpp"

#include "Renderwerk/Graphics/RenderPass/TestRenderPass.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"
#include "Renderwerk/Graphics/Pipeline/GfxGraphicsPipeline.hpp"
#include "Renderwerk/Platform/Window.hpp"

FTestRenderPass::FTestRenderPass(FGfxDevice* InGfxDevice, const FStringView& InDebugName)
	: IGfxRenderPass(InGfxDevice, InDebugName)
{
}

FTestRenderPass::~FTestRenderPass() = default;

void FTestRenderPass::CreateResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	const FAnsiString ShaderSource = R"(#pragma vertex VSMain
#pragma pixel PSMain

static const float3 VERTICES[] = {
    float3(-0.5, -0.5, 0.0),
    float3( 0.5, -0.5, 0.0),
    float3( 0.0,  0.5, 0.0)
};

static const float3 COLORS[] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

struct VertexOut
{
    float4 vPosition : SV_POSITION;
    float3 vColor : COLOR;
};

VertexOut VSMain(uint nVertexID : SV_VertexID)
{
    VertexOut Out = (VertexOut)0;

    Out.vPosition = float4(VERTICES[nVertexID], 1.0);
    Out.vColor = COLORS[nVertexID];

    return Out;
}

float4 PSMain(VertexOut Out) : SV_Target
{
    return float4(Out.vColor, 1.0);
})";

	const FGfxShaderModule VertexShader = GfxDevice->GetShaderCompiler()->CompilerShader(EGfxShaderType::Vertex, ShaderSource, L"VSMain");
	const FGfxShaderModule PixelShader = GfxDevice->GetShaderCompiler()->CompilerShader(EGfxShaderType::Pixel, ShaderSource, L"PSMain");

	FGfxGraphicsPipelineDesc GraphicsPipelineDesc = {};
	GraphicsPipelineDesc.Shaders[EGfxShaderType::Vertex] = VertexShader;
	GraphicsPipelineDesc.Shaders[EGfxShaderType::Pixel] = PixelShader;
	GraphicsPipelineDesc.Formats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
	GraphicsPipelineDesc.RootSignature = GfxDevice->CreateRootSignature();
	GraphicsPipeline = NewRef<FGfxGraphicsPipeline>(GfxDevice, GraphicsPipelineDesc);

	CreateRenderTarget(ResourceManager, GetEngine()->GetWindow()->GetState().ClientWidth, GetEngine()->GetWindow()->GetState().ClientHeight);
}

void FTestRenderPass::ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, const uint32 NewWidth, const uint32 NewHeight)
{
	ResourceManager->DestroyTexture(std::move(RenderTarget));
	CreateRenderTarget(ResourceManager, NewWidth, NewHeight);
}

void FTestRenderPass::ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	ResourceManager->DestroyTexture(std::move(RenderTarget));
	GraphicsPipeline.reset();
}

void FTestRenderPass::Render(const TRef<FGfxCommandList>& CommandList, const TRef<FGfxTexture>& BackBuffer)
{
	CommandList->ResourceBarrier(RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->SetRenderTarget(RenderTarget);

	constexpr float32 Color[4] = {1.0f, 0.5f, 0.5f, 1.0f};
	CommandList->ClearRenderTarget(RenderTarget, Color);

	CommandList->SetPipeline(GraphicsPipeline);
	CommandList->SetTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CommandList->SetViewport(RenderTarget->GetWidth(), RenderTarget->GetHeight());
	CommandList->SetScissor(RenderTarget->GetWidth(), RenderTarget->GetHeight());

	CommandList->DrawInstanced(3, 1);

	CommandList->ResourceBarrier(RenderTarget, D3D12_RESOURCE_STATE_COPY_SOURCE);
	CommandList->ResourceBarrier(BackBuffer, D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->CopyTextureToTexture(BackBuffer, RenderTarget);
}

void FTestRenderPass::CreateRenderTarget(const TRef<FGfxResourceManager>& ResourceManager, const uint32 Width, const uint32 Height)
{
	FGfxTextureDesc TextureDesc = {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.Usage = EGfxTextureUsage::RenderTarget;
	RenderTarget = ResourceManager->AllocateTexture(TextureDesc, TEXT("TestPassRenderTarget"));
}
