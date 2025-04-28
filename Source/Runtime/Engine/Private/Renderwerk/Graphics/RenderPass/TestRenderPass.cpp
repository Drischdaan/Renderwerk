#include "pch.hpp"

#include "Renderwerk/Graphics/RenderPass/TestRenderPass.hpp"

#include "imgui.h"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
#include "Renderwerk/Graphics/GfxRootSignature.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"
#include "Renderwerk/Graphics/Pipeline/GfxGraphicsPipeline.hpp"
#include "Renderwerk/Graphics/RenderPass/GfxImguiRenderPass.hpp"
#include "Renderwerk/Platform/Window.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"
#include "Renderwerk/Scene/Entity.hpp"
#include "Renderwerk/Scene/Scene.hpp"
#include "Renderwerk/Scene/SceneModule.hpp"
#include "Renderwerk/Scene/Components/CameraComponent.hpp"
#include "Renderwerk/Scene/Components/MeshComponent.hpp"
#include "Renderwerk/Scene/Components/TransformComponent.hpp"

FTestRenderPass::FTestRenderPass(FGfxDevice* InGfxDevice)
	: IGfxRenderPass(InGfxDevice, TEXT("TestRenderPass"))
{
}

FTestRenderPass::~FTestRenderPass() = default;

void FTestRenderPass::CreateResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	const FAnsiString ShaderSource = R"(#pragma vertex VSMain
#pragma pixel PSMain

cbuffer cb : register(b0)
{
    row_major float4x4 projectionMatrix : packoffset(c0);
    row_major float4x4 viewMatrix : packoffset(c4);
    row_major float4x4 modelMatrix : packoffset(c8);
};

struct VertexInput
{
    float3 inPos : POSITION;
    float3 inColor : COLOR;
};

struct VertexOut
{
    float4 vPosition : SV_POSITION;
    float3 vColor : COLOR;
};

VertexOut VSMain(VertexInput vertexInput)
{
    float3 inPos = vertexInput.inPos;
    float3 outColor = float3(vertexInput.inColor);
    float4 position = mul(mul(mul(projectionMatrix, viewMatrix), modelMatrix), float4(inPos, 1.0f));

    VertexOut output;
    output.vPosition = position;
    output.vColor = outColor;
    return output;
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
	GraphicsPipelineDesc.CullMode = EGfxCullMode::None;
	GraphicsPipelineDesc.bHasDepth = true;
	GraphicsPipelineDesc.bEnableDepthWrite = true;
	GraphicsPipelineDesc.DepthOperation = EGfxDepthOperation::Less;
	GraphicsPipelineDesc.DepthFormat = DXGI_FORMAT_D32_FLOAT;

	const TVector<EGfxRootType> RootTypes{
		EGfxRootType::ConstantBuffer,
	};
	GraphicsPipelineDesc.RootSignature = GfxDevice->CreateRootSignature(RootTypes, 0);
	GraphicsPipelineDesc.bUseShaderReflection = true;
	GraphicsPipeline = NewRef<FGfxGraphicsPipeline>(GfxDevice, GraphicsPipelineDesc);

	CreateRenderTarget(ResourceManager, GetEngine()->GetWindow()->GetState().ClientWidth, GetEngine()->GetWindow()->GetState().ClientHeight);
	CreateDepthStencil(ResourceManager, GetEngine()->GetWindow()->GetState().ClientWidth, GetEngine()->GetWindow()->GetState().ClientHeight);

	const TRef<FSceneModule> SceneModule = GetEngine()->GetModule<FSceneModule>();

	FCameraComponent* CameraComponent;
	const auto View = SceneModule->GetActiveScene()->CreateView<FCameraComponent>();
	View.each([&](FCameraComponent& TempCameraComponent)
	{
		if (TempCameraComponent.bIsMainCamera)
		{
			CameraComponent = &TempCameraComponent;
		}
	});
	CameraComponent->Camera.SetSize(static_cast<float32>(GetEngine()->GetWindow()->GetState().ClientWidth),
	                                static_cast<float32>(GetEngine()->GetWindow()->GetState().ClientHeight));

	EntityDestroyDelegateHandle = FScene::GetDeleteEntityDelegate().BindRaw(this, &FTestRenderPass::OnEntityDestroy);
}

void FTestRenderPass::ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, const uint32 NewWidth, const uint32 NewHeight)
{
	const TRef<FSceneModule> SceneModule = GetEngine()->GetModule<FSceneModule>();

	FCameraComponent* CameraComponent;
	const auto View = SceneModule->GetActiveScene()->CreateView<FCameraComponent>();
	View.each([&](FCameraComponent& TempCameraComponent)
	{
		if (TempCameraComponent.bIsMainCamera)
		{
			CameraComponent = &TempCameraComponent;
		}
	});
	CameraComponent->Camera.SetSize(static_cast<float32>(GetEngine()->GetWindow()->GetState().ClientWidth),
	                                static_cast<float32>(GetEngine()->GetWindow()->GetState().ClientHeight));
	ResourceManager->DestroyTexture(std::move(RenderTarget));
	ResourceManager->DestroyTexture(std::move(DepthStencil));
	CreateRenderTarget(ResourceManager, NewWidth, NewHeight);
	CreateDepthStencil(ResourceManager, NewWidth, NewHeight);
}

void FTestRenderPass::ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	FScene::GetDeleteEntityDelegate().Unbind(EntityDestroyDelegateHandle);

	const TRef<FSceneModule> SceneModule = GetEngine()->GetModule<FSceneModule>();
	const auto View = SceneModule->GetActiveScene()->CreateView<FMeshComponent>();
	View.each([&](FMeshComponent& MeshComponent)
	{
		ResourceManager->DestroyBuffer(std::move(MeshComponent.VertexBuffer));
		ResourceManager->DestroyBuffer(std::move(MeshComponent.IndexBuffer));
		ResourceManager->DestroyBuffer(std::move(MeshComponent.ConstantBuffer));
	});

	ResourceManager->DestroyTexture(std::move(DepthStencil));
	ResourceManager->DestroyTexture(std::move(RenderTarget));
	GraphicsPipeline.reset();
}

void FTestRenderPass::Render(const TRef<FGfxCommandList>& CommandList, const TRef<FGfxTexture>& BackBuffer)
{
	CommandList->ResourceBarrier(RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(DepthStencil, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	CommandList->SetRenderTarget(RenderTarget, DepthStencil);

	constexpr float32 Color[4] = {1.0f, 0.5f, 0.5f, 1.0f};
	CommandList->ClearRenderTarget(RenderTarget, Color);
	CommandList->ClearDepthStencil(DepthStencil);

	FGfxConstantBuffer ConstantBufferValues = {};
	const TRef<FSceneModule> SceneModule = GetEngine()->GetModule<FSceneModule>();
	ID3D12GraphicsCommandList10* NativeCommandList = CommandList->GetNativeObject<ID3D12GraphicsCommandList10>(NativeObjectIds::D3D12_CommandList);

	ID3D12DescriptorHeap* DescriptorHeaps[] = {GfxDevice->GetSRVDescriptorHeap()->GetNativeObject<ID3D12DescriptorHeap>(NativeObjectIds::D3D12_DescriptorHeap)};
	NativeCommandList->SetDescriptorHeaps(1, DescriptorHeaps);

	CommandList->SetPipeline(GraphicsPipeline);
	CommandList->SetTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CommandList->SetViewport(RenderTarget->GetWidth(), RenderTarget->GetHeight());
	CommandList->SetScissor(RenderTarget->GetWidth(), RenderTarget->GetHeight());

	const auto View = SceneModule->GetActiveScene()->CreateView<FMeshComponent, const FTransformComponent>();
	View.each([&](FMeshComponent& MeshComponent, const FTransformComponent& TransformComponent)
	{
		PROFILE_SCOPE("RenderMesh");

		if (!MeshComponent.VertexBuffer)
		{
			const size64 Size = sizeof(FVertex) * MeshComponent.Vertices.size();
			MeshComponent.VertexBuffer = GfxDevice->GetResourceManager()->AllocateBuffer(EGfxBufferType::Vertex, Size, sizeof(FVertex));
			MeshComponent.VertexBuffer->SetData(MeshComponent.Vertices.data(), Size);
		}
		if (!MeshComponent.IndexBuffer)
		{
			const size64 Size = sizeof(uint32) * MeshComponent.Indices.size();
			MeshComponent.IndexBuffer = GfxDevice->GetResourceManager()->AllocateBuffer(EGfxBufferType::Index, Size, sizeof(uint32));
			MeshComponent.IndexBuffer->SetData(MeshComponent.Indices.data(), Size);
		}
		if (!MeshComponent.ConstantBuffer)
		{
			MeshComponent.ConstantBuffer = GfxDevice->GetResourceManager()->AllocateBuffer(EGfxBufferType::Constant, sizeof(FGfxConstantBuffer),
			                                                                               sizeof(FGfxConstantBuffer));
		}

		FCameraComponent* CameraComponent;
		FTransformComponent* CameraTransformComponent;
		const auto CameraView = SceneModule->GetActiveScene()->CreateView<FCameraComponent, FTransformComponent>();
		CameraView.each([&](FCameraComponent& TempCameraComponent, FTransformComponent& TempTransformComponent)
		{
			if (TempCameraComponent.bIsMainCamera)
			{
				CameraComponent = &TempCameraComponent;
				CameraTransformComponent = &TempTransformComponent;
			}
		});
		CameraComponent->Camera.Update();
		ConstantBufferValues.ProjectionMatrix = CameraComponent->Camera.GetProjectionMatrix();

		const glm::mat4 CameraTransform = CameraTransformComponent->GetTransform();
		ConstantBufferValues.ViewMatrix = glm::inverse(CameraTransform);

		ConstantBufferValues.ModelMatrix = TransformComponent.GetTransform();
		MeshComponent.ConstantBuffer->CopyMappedData(&ConstantBufferValues, sizeof(ConstantBufferValues));

		const D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = MeshComponent.ConstantBuffer->GetSRVDescriptorHandle().GetGPUHandle();
		NativeCommandList->SetGraphicsRootDescriptorTable(0, GPUHandle);

		CommandList->ResourceBarrier(MeshComponent.VertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		CommandList->ResourceBarrier(MeshComponent.IndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		const D3D12_VERTEX_BUFFER_VIEW VertexBufferView = MeshComponent.VertexBuffer->GetVertexBufferView();
		NativeCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);

		const D3D12_INDEX_BUFFER_VIEW IndexBufferView = MeshComponent.IndexBuffer->GetIndexBufferView();
		NativeCommandList->IASetIndexBuffer(&IndexBufferView);
		NativeCommandList->DrawIndexedInstanced(static_cast<uint32>(MeshComponent.Indices.size()), 1, 0, 0, 0);

		CommandList->ResourceBarrier(MeshComponent.VertexBuffer, D3D12_RESOURCE_STATE_COMMON);
		CommandList->ResourceBarrier(MeshComponent.IndexBuffer, D3D12_RESOURCE_STATE_COMMON);
	});

	CommandList->ResourceBarrier(DepthStencil, D3D12_RESOURCE_STATE_COMMON);
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

void FTestRenderPass::CreateDepthStencil(const TRef<FGfxResourceManager>& ResourceManager, const uint32 Width, const uint32 Height)
{
	FGfxTextureDesc TextureDesc = {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.Usage = EGfxTextureUsage::DepthTarget;
	TextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencil = ResourceManager->AllocateTexture(TextureDesc, TEXT("TestPassDepthStencil"));
}

void FTestRenderPass::OnEntityDestroy(FEntity& Entity) const
{
	if (Entity.HasComponents<FMeshComponent>())
	{
		FMeshComponent& MeshComponent = Entity.GetComponent<FMeshComponent>();
		GfxDevice->GetResourceManager()->DestroyBuffer(std::move(MeshComponent.VertexBuffer));
		GfxDevice->GetResourceManager()->DestroyBuffer(std::move(MeshComponent.IndexBuffer));
	}
}
