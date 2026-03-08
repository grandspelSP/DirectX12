//=========================================================
// @file TextureRender.cpp
// @auth Kota Tamaoki
// @date 2022/12/03
//=========================================================

#include "TextureRender.h"
#include "../DirectXTex/d3dx12.h"
#include "../WICTextureLoader/WICTextureLoader12.h"
#include "../Device.h"
#include "../DirectXIncluder.hpp"
#include "../Camera.h"

//--------------------------------------------------------------------------------------
TextureRender::TextureRender()
{
    mMatrix = XMMatrixIdentity();

}
//--------------------------------------------------------------------------------------
HRESULT TextureRender::Init(const char* texturePath)
{
    Device::Vertex vertexArray[4];
    vertexArray[0].position = { -1.0f,  1.0f, 0.0f };
    vertexArray[0].normal = { 0.0f, 0.0f, -1.f };
    vertexArray[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    vertexArray[0].uv = { 0.0f, 0.0f };
    vertexArray[1].position = { 1.0f,  1.0f, 0.0f };
    vertexArray[1].normal = { 0.0f, 0.0f, -1.0f };
    vertexArray[1].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    vertexArray[1].uv = { 1.f, 0.0f };
    vertexArray[2].position = { 1.0f, -1.0f, 0.0f };
    vertexArray[2].normal = { 0.0f, 0.0f, -1.0f };
    vertexArray[2].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    vertexArray[2].uv = { 1.0f, 1.0f };
    vertexArray[3].position = { -1.0f, -1.0f, 0.0f };
    vertexArray[3].normal = { 0.0f, 0.0f, -1.0f };
    vertexArray[3].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    vertexArray[3].uv = { 0.0f, 1.0f };
    uint32_t    indices[] = { 0, 1, 2, 0, 2, 3 };
    mVertexBufferSize = sizeof(vertexArray);
    mIndexBufferSize = sizeof(indices);
    mTexturePath = texturePath;

    // 頂点バッファ生成
    {
        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CreationNodeMask = 1;
        heapProperties.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = mVertexBufferSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        if (FAILED(GetDevice()->mD3D12Device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mVertexBuffer))))
        {
            return E_FAIL;
        }

        // データ設定
        UINT8* pVertexDataBegin;
        D3D12_RANGE readRange = { 0, 0 };
        if (FAILED(mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin))))
        {
            return E_FAIL;
        }
        memcpy(pVertexDataBegin, vertexArray, mVertexBufferSize);
        mVertexBuffer->Unmap(0, nullptr);
    }

    // インデックスバッファ生成
    {
        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CreationNodeMask = 1;
        heapProperties.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = mIndexBufferSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        if (FAILED(GetDevice()->mD3D12Device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mIndexBuffer))))
        {
            return E_FAIL;
        }

        // データ設定
        UINT8* pIndexDataBegin;
        if (FAILED(mIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pIndexDataBegin))))
        {
            return E_FAIL;
        }
        memcpy(pIndexDataBegin, indices, mIndexBufferSize);
        mIndexBuffer->Unmap(0, nullptr);
    }

    // 定数バッファ生成
    {
        // 定数バッファリソース生成
        D3D12_HEAP_PROPERTIES constantProperties = {};
        constantProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        constantProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        constantProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        constantProperties.CreationNodeMask = 1;
        constantProperties.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC constantResource = {};
        constantResource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        constantResource.Alignment = 0;
        constantResource.Width = 256;
        constantResource.Height = 1;
        constantResource.DepthOrArraySize = 1;
        constantResource.MipLevels = 1;
        constantResource.Format = DXGI_FORMAT_UNKNOWN;
        constantResource.SampleDesc = { 1, 0 };
        constantResource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        constantResource.Flags = D3D12_RESOURCE_FLAG_NONE;

        if (FAILED(GetDevice()->mD3D12Device->CreateCommittedResource(&constantProperties, D3D12_HEAP_FLAG_NONE, &constantResource, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mConstantBuffer))))
        {
            return E_FAIL;
        }
    }
    
    // テクスチャ生成
    if (!mTexturePath.empty())
    {
        if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
        {
            return E_FAIL;
        }

        // 読み込み
        std::unique_ptr<uint8_t[]>    wicData;
        D3D12_SUBRESOURCE_DATA      subresourceData;
        if (FAILED(LoadWICTextureFromFile(GetDevice()->mD3D12Device.Get(), mTexturePath.wstring().data(), &mTexture, wicData, subresourceData)))
        {
            return E_FAIL;
        }

        D3D12_RESOURCE_DESC textureResourceDesc = mTexture->GetDesc();
        UINT64 uploadBufferSize = GetRequiredIntermediateSize(mTexture.Get(), 0, 1);

        // GPUアップロードバッファ生成
        D3D12_HEAP_PROPERTIES tmpHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC   tmpResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        if (FAILED(GetDevice()->mD3D12Device->CreateCommittedResource(
            &tmpHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &tmpResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mTextureUploadHeap))))
        {
            return E_FAIL;
        }

        // テクスチャ情報更新
        UpdateSubresources(GetDevice()->mCommandList.Get(), mTexture.Get(), mTextureUploadHeap.Get(), 0, 0, 1, &subresourceData);
        D3D12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        GetDevice()->mCommandList->ResourceBarrier(1, &resourceBarrier);

        // テクスチャデスクリプタヒープ生成
        D3D12_DESCRIPTOR_HEAP_DESC textureHeapDesc = {};
        textureHeapDesc.NumDescriptors = 1;
        textureHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        textureHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        textureHeapDesc.NodeMask = 0;

        if (FAILED(GetDevice()->mD3D12Device->CreateDescriptorHeap(&textureHeapDesc, IID_PPV_ARGS(&mTextureHeap))))
        {
            return E_FAIL;
        }

        // シェーダーリソースビュー生成
        D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
        shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shaderResourceViewDesc.Format = textureResourceDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        GetDevice()->mD3D12Device->CreateShaderResourceView(mTexture.Get(), &shaderResourceViewDesc, mTextureHeap->GetCPUDescriptorHandleForHeapStart());

        // アンロード
        GetDevice()->mCommandList->DiscardResource(mTextureUploadHeap.Get(), nullptr);
    }

    return S_OK;
}
//--------------------------------------------------------------------------------------
void TextureRender::SetPosition(const XMFLOAT3& position) {
    mMatrix.r[3] = XMVectorSet( position.x / static_cast<float>(GetDevice()->WINDOW_WIDTH),
                                position.y / static_cast<float>(GetDevice()->WINDOW_HEIGHT), 
                                position.z, 1.0f);
}
//--------------------------------------------------------------------------------------
void TextureRender::SetRotation(const XMFLOAT3& rotation) {
    XMMATRIX rotX = XMMatrixRotationX(rotation.x);
    XMMATRIX rotY = XMMatrixRotationY(rotation.y);
    XMMATRIX rotZ = XMMatrixRotationZ(rotation.z);
    mMatrix.r[0] = rotX.r[0] * rotY.r[0] * rotZ.r[0];
    mMatrix.r[1] = rotX.r[1] * rotY.r[1] * rotZ.r[1];
    mMatrix.r[2] = rotX.r[2] * rotY.r[2] * rotZ.r[2];
}
//--------------------------------------------------------------------------------------
void TextureRender::SetScale(const XMFLOAT3& scale)
{
    mMatrix.r[0] = XMVectorSetX(mMatrix.r[0], scale.x / static_cast<float>(GetDevice()->WINDOW_WIDTH));
    mMatrix.r[1] = XMVectorSetY(mMatrix.r[1], scale.y / static_cast<float>(GetDevice()->WINDOW_HEIGHT));
	mMatrix.r[2] = XMVectorSetZ(mMatrix.r[2], scale.z);
}
//--------------------------------------------------------------------------------------
HRESULT TextureRender::RenderWICTexture()
{
    auto& command_list = GetDevice()->mCommandList;

    XMFLOAT4X4 mat;
    XMStoreFloat4x4(&mat, XMMatrixTranspose(mMatrix /** GetCamera()->GetViewProjection()*/));
    XMFLOAT4X4* pBuffer = nullptr;
    mConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pBuffer));
    *pBuffer = mat;
    mConstantBuffer->Unmap(0, nullptr);

    command_list->SetGraphicsRootSignature(GetDevice()->mRootSignature.Get());
    command_list->SetPipelineState(GetDevice()->mPipelineState.Get());

    // 定数バッファをシェーダレジスタに設定
    command_list->SetGraphicsRootConstantBufferView(0, mConstantBuffer->GetGPUVirtualAddress());

    // テクスチャ設定
    ID3D12DescriptorHeap* pTextureHeaps[] = { mTextureHeap.Get() };
    command_list->SetDescriptorHeaps(_countof(pTextureHeaps), pTextureHeaps);
    command_list->SetGraphicsRootDescriptorTable(1, mTextureHeap->GetGPUDescriptorHandleForHeapStart());

    // 頂点バッファビュー設定
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    vertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Device::Vertex);

    vertexBufferView.SizeInBytes = mVertexBufferSize;
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command_list->IASetVertexBuffers(0, 1, &vertexBufferView);

    // インデックスバッファビュー設定
    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    indexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    indexBufferView.SizeInBytes = mIndexBufferSize;
    command_list->IASetIndexBuffer(&indexBufferView);
    command_list->DrawIndexedInstanced((mIndexBufferSize / sizeof(uint32_t)), 1, 0, 0, 0);

	return S_OK;
}
//--------------------------------------------------------------------------------------