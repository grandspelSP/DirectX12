//=========================================================
// @file TextureRender.h
// @auth Kota Tamaoki
// @date 2022/12/03
//=========================================================

#pragma once
#include <windows.h>
#include <vector>
#include <tchar.h>
#include <wrl.h>		// Microsoft::WRL::ComPtr
#include "../DIrectXIncluder.hpp"
#include "ComponentBase.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class TextureRender : public ComponentBase
{
public:
	TextureRender();
	~TextureRender() = default;

	// 初期化
	HRESULT Init();

	// 描画
	HRESULT RenderWICTexture();

	ComPtr<ID3D12Resource> getVertexBuffer() {
		return mVertexBuffer;
	}
	D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() {
		return mVertexBufferView;
	}

private:
	// リソース
	ComPtr<ID3D12Resource>				mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			mVertexBufferView;

	ComPtr<ID3D12Resource> mIndexBuffer;
	ComPtr<ID3D12Resource> mConstantBuffer;

	UINT                    mVertexBufferSize;
	UINT                    mIndexBufferSize;

	XMMATRIX                mMatrix;

	// テクスチャ情報
	ComPtr<ID3D12Resource> mTexture;
	ComPtr<ID3D12Resource> mTextureUploadHeap;
	ComPtr<ID3D12DescriptorHeap> mTextureHeap;
};