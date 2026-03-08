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
#include <filesystem>

using namespace DirectX;
using namespace Microsoft::WRL;

class TextureRender : public ComponentBase
{
public:
	TextureRender();
	~TextureRender() = default;

	// 初期化
	HRESULT Init(const char* texturePath);

	// 更新
	void SetPosition(const XMFLOAT3& position) {
		mMatrix.r[3] = XMVectorSet(position.x, position.y, position.z, 1.0f);
	}
	void SetRotation(const XMFLOAT3& rotation) {
		XMMATRIX rotX = XMMatrixRotationX(rotation.x);
		XMMATRIX rotY = XMMatrixRotationY(rotation.y);
		XMMATRIX rotZ = XMMatrixRotationZ(rotation.z);
		mMatrix.r[0] = rotX.r[0] * rotY.r[0] * rotZ.r[0];
		mMatrix.r[1] = rotX.r[1] * rotY.r[1] * rotZ.r[1];
		mMatrix.r[2] = rotX.r[2] * rotY.r[2] * rotZ.r[2];
	}

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
	D3D12_VERTEX_BUFFER_VIEW			mVertexBufferView{};

	ComPtr<ID3D12Resource> mIndexBuffer;
	ComPtr<ID3D12Resource> mConstantBuffer;

	UINT                    mVertexBufferSize = 0;
	UINT                    mIndexBufferSize = 0;

	XMMATRIX                mMatrix;

	// テクスチャ情報
	std::filesystem::path mTexturePath;
	ComPtr<ID3D12Resource> mTexture;
	ComPtr<ID3D12Resource> mTextureUploadHeap;
	ComPtr<ID3D12DescriptorHeap> mTextureHeap;
};