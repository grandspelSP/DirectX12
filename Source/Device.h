//=========================================================
// @file Device.h
// @auth Kota Tamaoki
// @date 2026/02/23
//=========================================================

#pragma once

#include <windows.h>
#include "DirectXIncluder.hpp"
#include <wrl.h>		// Microsoft::WRL::ComPtr
#include <vector>

using namespace Microsoft::WRL;

namespace {
	constexpr int  FRAME_COUNT{ 2 };		// レンダーターゲットサイズ
}

class Device
{
private:
	Device() = default;
	~Device() = default;

public:
	static Device* GetInstance()
	{
		if (!mInstance) {
			mInstance = new Device();
		}
		return mInstance;
	}

	HRESULT InitDevice(HWND hWnd);

	HRESULT RenderBegin();
	HRESULT RenderEnd();

	HRESULT WaitForPreviousFrame();

	HRESULT Shutdown();

	const LONG WINDOW_WIDTH{ 1280 };	// ウィンドウ幅
	const LONG WINDOW_HEIGHT{ 720 };	// ウィンドウ高さ

public:
	struct Vertex {
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	uv;
	};

	ComPtr<IDXGISwapChain3>				mSwapChain;
	ComPtr<ID3D12Device>				mD3D12Device;
	ComPtr<ID3D12GraphicsCommandList>	mCommandList;
	std::vector<ComPtr<ID3D12Resource>>	mRenderTargets;
	ComPtr<ID3D12PipelineState>			mPipelineState;
	ComPtr<ID3D12DescriptorHeap>		mRtvHeap;
	ComPtr<ID3D12DescriptorHeap>		mSrvHeapForImgui;
	UINT								mRtvDescriptorSize = 0;
	ComPtr<ID3D12RootSignature>			mRootSignature;
	ComPtr<ID3D12CommandAllocator>		mCommandAllocator;
	ComPtr<ID3D12CommandQueue>			mCommandQueue;
	D3D12_VIEWPORT						mViewport{ 0.0f, 0.0f, static_cast<FLOAT>(WINDOW_WIDTH), static_cast<FLOAT>(WINDOW_HEIGHT) };
	D3D12_RECT							mScissorRect{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	// 同期オブジェクト
	UINT				mFrameIndex = 0;
	HANDLE				mFenceEvent;
	ComPtr<ID3D12Fence>	mFence;
	UINT64				mFenceValue;

	// リソース
	//ComPtr<ID3D12Resource>				mVertexBuffer;
	//D3D12_VERTEX_BUFFER_VIEW			mVertexBufferView;

	// ビューポートのアスペクト比
	float mAspectRatio{ static_cast<FLOAT>(WINDOW_WIDTH) / static_cast<FLOAT>(WINDOW_HEIGHT) };

private:
	static Device* mInstance;
};

inline Device* GetDevice() { return Device::GetInstance(); }