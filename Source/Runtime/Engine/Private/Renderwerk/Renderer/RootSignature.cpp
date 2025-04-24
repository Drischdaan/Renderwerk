#include "pch.hpp"

#include "Renderwerk/Renderer/RootSignature.hpp"

#include "Renderwerk/Renderer/Device.hpp"

FRootSignature::FRootSignature(FDevice* InDevice)
	: Device(InDevice)
{
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
	RootSignatureDesc.NumParameters = 0;
	RootSignatureDesc.pParameters = nullptr;
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	TObjectHandle<ID3DBlob> RootSignatureBlob;
	TObjectHandle<ID3DBlob> ErrorBlob;
	HRESULT Result = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &RootSignatureBlob, &ErrorBlob);
	if (ErrorBlob)
	{
		RW_LOG(Error, "Unable to serialize root signature! {}", ErrorBlob->GetBufferPointer());
	}
	RW_VERIFY_ID(Result);

	Result = Device->GetHandle()->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
	RW_VERIFY_ID(Result);
}

FRootSignature::FRootSignature(FDevice* InDevice, const TVector<ERootType>& RootTypes, const size64 InPushConstantSize)
	: Device(InDevice)
{
	TVector<D3D12_ROOT_PARAMETER> Parameters(RootTypes.size());
	TVector<D3D12_DESCRIPTOR_RANGE> Ranges(RootTypes.size());

	for (size64 Index = 0; Index < RootTypes.size(); ++Index)
	{
		D3D12_DESCRIPTOR_RANGE& DescriptorRange = Ranges.at(Index);
		DescriptorRange.NumDescriptors = 1;
		DescriptorRange.BaseShaderRegister = static_cast<uint32>(Index);
		DescriptorRange.RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(RootTypes.at(Index));
		DescriptorRange.RegisterSpace = 0;

		D3D12_ROOT_PARAMETER& RootParameter = Parameters.at(Index);
		if (RootTypes.at(Index) == ERootType::PushConstant)
		{
			RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			RootParameter.Constants.ShaderRegister = static_cast<uint32>(Index);
			RootParameter.Constants.RegisterSpace = 0;
			RootParameter.Constants.Num32BitValues = static_cast<uint32>(InPushConstantSize) / 4;
		}
		else
		{
			RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			RootParameter.DescriptorTable.NumDescriptorRanges = 1;
			RootParameter.DescriptorTable.pDescriptorRanges = &DescriptorRange;
			RootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		}
	}

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc = {};
	RootSignatureDesc.NumParameters = static_cast<uint32>(Parameters.size());
	RootSignatureDesc.pParameters = Parameters.data();
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
		| D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

	TObjectHandle<ID3DBlob> RootSignatureBlob;
	TObjectHandle<ID3DBlob> ErrorBlob;
	HRESULT Result = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &RootSignatureBlob, &ErrorBlob);
	if (ErrorBlob)
	{
		RW_LOG(Error, "Unable to serialize root signature! {}", ErrorBlob->GetBufferPointer());
	}
	RW_VERIFY_ID(Result);

	Result = Device->GetHandle()->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
	RW_VERIFY_ID(Result);
}

FRootSignature::~FRootSignature()
{
	RootSignature.Reset();
}
