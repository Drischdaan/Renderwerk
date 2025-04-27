#include "pch.hpp"

#include "Renderwerk/Graphics/RenderPass/TestRenderPass.hpp"

#include "imgui.h"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Graphics/GfxCommandList.hpp"
#include "Renderwerk/Graphics/GfxDevice.hpp"
#include "Renderwerk/Graphics/GfxResourceManager.hpp"
#include "Renderwerk/Graphics/GfxTexture.hpp"
#include "Renderwerk/Graphics/Pipeline/GfxGraphicsPipeline.hpp"
#include "Renderwerk/Graphics/RenderPass/GfxImguiRenderPass.hpp"
#include "Renderwerk/Platform/Window.hpp"

FTestRenderPass::FTestRenderPass(FGfxDevice* InGfxDevice)
	: IGfxRenderPass(InGfxDevice, TEXT("TestRenderPass"))
{
}

FTestRenderPass::~FTestRenderPass() = default;

void FTestRenderPass::CreateResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	const FAnsiString ShaderSource = R"(#pragma vertex VSMain
#pragma pixel PSMain

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
	float3 inColor = vertexInput.inColor;
    float3 inPos = vertexInput.inPos;
    float3 outColor = inColor;
    float4 position = float4(inPos, 1.0f);

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
	GraphicsPipelineDesc.RootSignature = GfxDevice->CreateRootSignature();
	GraphicsPipelineDesc.bUseShaderReflection = true;
	GraphicsPipeline = NewRef<FGfxGraphicsPipeline>(GfxDevice, GraphicsPipelineDesc);

	CreateRenderTarget(ResourceManager, GetEngine()->GetWindow()->GetState().ClientWidth, GetEngine()->GetWindow()->GetState().ClientHeight);


	Vertices = {
		{.Positon = {0.5f, -0.5f, 0.0f}, .Color = {1.0f, 0.0f, 0.0f}},
		{.Positon = {-0.5f, -0.5f, 0.0f}, .Color = {0.0f, 1.0f, 0.0f}},
		{.Positon = {0.0f, 0.5f, 0.0f}, .Color = {0.0f, 0.0f, 1.0f}}
	};

	constexpr size64 VertexSize = sizeof(FVertex);
	const size64 Size = VertexSize * Vertices.size();
	VertexBuffer = ResourceManager->AllocateBuffer(EGfxBufferType::Vertex, Size, VertexSize);
	VertexBuffer->SetData(Vertices.data(), Size);

	Indices = {0, 1, 2};
	IndexBuffer = ResourceManager->AllocateBuffer(EGfxBufferType::Index, sizeof(uint32) * Indices.size(), sizeof(uint32));
	IndexBuffer->SetData(Indices.data(), Indices.size());

	TestHandle = FGfxImguiRenderPass::GetImguiRenderDelegate().BindRaw(this, &FTestRenderPass::OnImguiRender);
}

void FTestRenderPass::ResizeResources(const TRef<FGfxResourceManager>& ResourceManager, const uint32 NewWidth, const uint32 NewHeight)
{
	ResourceManager->DestroyTexture(std::move(RenderTarget));
	CreateRenderTarget(ResourceManager, NewWidth, NewHeight);
}

void FTestRenderPass::ReleaseResources(const TRef<FGfxResourceManager>& ResourceManager)
{
	FGfxImguiRenderPass::GetImguiRenderDelegate().Unbind(TestHandle);

	ResourceManager->DestroyBuffer(std::move(IndexBuffer));
	ResourceManager->DestroyBuffer(std::move(VertexBuffer));
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

	CommandList->ResourceBarrier(VertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	CommandList->ResourceBarrier(IndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	ID3D12GraphicsCommandList10* NativeCommandList = CommandList->GetNativeObject<ID3D12GraphicsCommandList10>(NativeObjectIds::D3D12_CommandList);
	const D3D12_VERTEX_BUFFER_VIEW VertexBufferView = VertexBuffer->GetVertexBufferView();
	NativeCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);

	const D3D12_INDEX_BUFFER_VIEW IndexBufferView = IndexBuffer->GetIndexBufferView();
	NativeCommandList->IASetIndexBuffer(&IndexBufferView);
	NativeCommandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

	CommandList->ResourceBarrier(VertexBuffer, D3D12_RESOURCE_STATE_COMMON);
	CommandList->ResourceBarrier(IndexBuffer, D3D12_RESOURCE_STATE_COMMON);

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

void FTestRenderPass::OnImguiRender()
{
	if (!ImGui::Begin("Vertex Editor"))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginTable("VertexTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
	{
		ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 30.0f);
		ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, 60.0f);
		ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, 60.0f);
		ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, 60.0f);
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();

		for (size64 Index = 0; Index < Vertices.size(); Index++)
		{
			FVertex& Vertex = Vertices.at(Index);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%zu", Index);

			ImGui::TableSetColumnIndex(1);
			ImGui::PushID(static_cast<int>(Index * 10 + 0));
			ImGui::PushItemWidth(60.0f);
			ImGui::DragFloat("##X", &Vertex.Positon[0], 0.01f);
			ImGui::PopID();

			ImGui::TableSetColumnIndex(2);
			ImGui::PushID(static_cast<int>(Index * 10 + 1));
			ImGui::PushItemWidth(60.0f);
			ImGui::DragFloat("##Y", &Vertex.Positon[1], 0.01f);
			ImGui::PopID();

			ImGui::TableSetColumnIndex(3);
			ImGui::PushID(static_cast<int>(Index * 10 + 2));
			ImGui::PushItemWidth(60.0f);
			ImGui::DragFloat("##Z", &Vertex.Positon[2], 0.01f);
			ImGui::PopID();

			ImGui::TableSetColumnIndex(4);
			ImGui::PushID(static_cast<int>(Index * 10 + 6));
			if (ImGui::Button("Color"))
			{
				ImGui::OpenPopup("ColorPicker");
			}

			if (ImGui::BeginPopup("ColorPicker"))
			{
				ImGui::ColorPicker3("##picker", Vertex.Color);
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	if (ImGui::Button("Apply"))
	{
		VertexBuffer->SetData(Vertices.data(), sizeof(FVertex) * Vertices.size());
	}

	ImGui::End();
}
