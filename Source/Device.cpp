//=========================================================
// @file Device.cpp
// @auth Kota Tamaoki
// @date 2026/02/23
//=========================================================

#include "Device.h"
#include "DirectXTex/d3dx12.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx12.h"
#include "Imgui/imgui_impl_win32.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

Device* Device::mInstance = nullptr;

HRESULT Device::InitDevice(HWND hWnd)
{
	UINT dxgi_factory_flags = 0;

#if defined(_DEBUG)
	// DirectX12のデバッグレイヤーを有効
	{
		ComPtr<ID3D12Debug>	debug_controller;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf()))))
		{
			debug_controller->EnableDebugLayer();

			// 追加のデバッグレイヤーを有効
			dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	// ファクトリを作成
	ComPtr<IDXGIFactory4> factory;
	if (FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(factory.GetAddressOf()))))
	{
		MessageBox(nullptr, L"ファクトリの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// DirectX12がサポートする利用可能なハードウェアアダプタを検索し取得
	HRESULT hr;
	ComPtr<IDXGIAdapter1> hardware_adapter = nullptr;
	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapter_index, adapter.GetAddressOf()); ++adapter_index)
	{
		DXGI_ADAPTER_DESC1 adapter_desc;
		adapter->GetDesc1(&adapter_desc);

		// どちらかがFALSEならFALSEでスルー
		if (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		// アダプタがDirectX12に対応しているか確認
		hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			// デバイスを作成
			if (FAILED(D3D12CreateDevice(hardware_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mD3D12Device.GetAddressOf()))))
			{
				MessageBox(nullptr, L"選択したハードウェアデバイスがDirectX12に対応していません。", nullptr, MB_OK | MB_ICONERROR);
				return E_FAIL;
			}
			break;
		}
	}

	// 関連付け解除
	hardware_adapter = adapter.Detach();

	// ハードウェアで対応できない場合はWARPデバイス(ソフトウェア)の方を用いる
	if (FAILED(hr))
	{
		ComPtr<IDXGIAdapter> warp_adapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(warp_adapter.GetAddressOf()));
		if (FAILED(D3D12CreateDevice(warp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mD3D12Device.GetAddressOf()))))
		{
			MessageBox(nullptr, L"選択したWARPデバイスまでもがDirectX12に対応していません。", nullptr, MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
	}

	// コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// コマンドキューを作成
	if (FAILED(mD3D12Device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(mCommandQueue.GetAddressOf()))))
	{
		MessageBox(nullptr, L"コマンドキューの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
	swap_chain_desc.BufferCount = FRAME_COUNT;
	swap_chain_desc.Width = WINDOW_WIDTH;
	swap_chain_desc.Height = WINDOW_HEIGHT;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;       // フロントバッファとバックバッファの入れ替え方法
	swap_chain_desc.SampleDesc.Count = 1;

	// スワップチェインを作成
	ComPtr<IDXGISwapChain1> swap_chain;
	if (FAILED(factory->CreateSwapChainForHwnd(mCommandQueue.Get(), hWnd, &swap_chain_desc, nullptr, nullptr, swap_chain.GetAddressOf())))
	{
		MessageBox(nullptr, L"スワップチェインの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// Alt+Enterによる全画面遷移をできないようにする
	if (FAILED(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
	{
		MessageBox(nullptr, L"画面の設定ができません。", nullptr, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// スワップチェインをキャスト
	swap_chain.As(&mSwapChain);

	// バックバッファのインデックスを格納
	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	{
		// レンダーターゲットビュー用のディスクリプタヒープの設定
		D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
		rtv_heap_desc.NumDescriptors = FRAME_COUNT;
		rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		// RTVディスクリプタヒープを作成
		if (FAILED(mD3D12Device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(mRtvHeap.GetAddressOf()))))
		{
			MessageBox(nullptr, L"RTVディスクリプタヒープの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
			return E_FAIL;
		}

		// ディスクリプタのサイズを取得
		mRtvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		mRenderTargets.resize(FRAME_COUNT);

		// フレームリソースのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

		// フレームバッファとバックバッファのレンダーターゲットビューを作成
		for (UINT i = 0; i < FRAME_COUNT; i++)
		{
			// RTVバッファを取得
			if (FAILED(mSwapChain->GetBuffer(i, IID_PPV_ARGS(mRenderTargets[i].GetAddressOf()))))
			{
				MessageBox(nullptr, L"RTVバッファの取得に失敗。", nullptr, MB_OK | MB_ICONERROR);
				return E_FAIL;
			}

			// レンダーターゲットビューを作成
			mD3D12Device->CreateRenderTargetView(mRenderTargets[i].Get(), nullptr, rtv_handle);

			// ハンドルのオフセット
			rtv_handle.Offset(1, mRtvDescriptorSize);
		}
	}

	// コマンドアロケーターを作成
	if (FAILED(mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf()))))
	{
		MessageBox(nullptr, L"コマンドアロケータの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// ルートシグネチャを作成
	{
		//D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
		//feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		//// サポートバージョンチェック
		//if (FAILED(mD3D12Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data)))) {
		//	feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		//}

		// ディスクリプタ区間の設定
		CD3DX12_DESCRIPTOR_RANGE ranges[1];
		//ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		ranges[0].NumDescriptors = 1;
		ranges[0].BaseShaderRegister = 0;
		ranges[0].RegisterSpace = 0;
		ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER root_param[2];
		root_param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		root_param[0].Descriptor.ShaderRegister = 0;
		root_param[0].Descriptor.RegisterSpace = 0;
		root_param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		root_param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		root_param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		root_param[1].DescriptorTable.NumDescriptorRanges = 1;
		root_param[1].DescriptorTable.pDescriptorRanges = &ranges[0];

		// サンプラの設定
		D3D12_STATIC_SAMPLER_DESC sampler_desc = {};
		sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 0;
		sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		sampler_desc.MinLOD = 0.0f;
		sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
		sampler_desc.ShaderRegister = 0;
		sampler_desc.RegisterSpace = 0;
		sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// ルート署名の設定
		CD3DX12_ROOT_SIGNATURE_DESC vrs_desc;
		vrs_desc.NumParameters = _countof(root_param);
		vrs_desc.pParameters = root_param;
		vrs_desc.NumStaticSamplers = 1;
		vrs_desc.pStaticSamplers = &sampler_desc;
		vrs_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// ルートシグネチャを作成
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		D3D12SerializeRootSignature(&vrs_desc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf());
		if (FAILED(mD3D12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature))))
		{
			MessageBox(nullptr, L"ルートシグネチャの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
			return E_FAIL;
		}
	}

	// InitShader
	{
		ComPtr<ID3DBlob>	vertex_shader;
		ComPtr<ID3DBlob>	pixel_shader;

#if defined(_DEBUG)
		// グラフィックデバッグツールによるシェーダーのデバッグを有効にする
		UINT	compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT	compile_flags = 0;
#endif

		if (FAILED(D3DCompileFromFile(
			L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compile_flags, 0, &vertex_shader, nullptr))) {
			return E_FAIL;
		}
		if (FAILED(D3DCompileFromFile(
			L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compile_flags, 0, &pixel_shader, nullptr))) {
			return E_FAIL;
		}

		// 頂点入力レイアウトを定義
		D3D12_INPUT_ELEMENT_DESC	input_element_descs[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT     , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT     , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT  , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT        , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		// グラフィックスパイプラインの状態オブジェクトを作成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC	pso_desc = {};
		pso_desc.InputLayout = { input_element_descs, _countof(input_element_descs) };
		pso_desc.pRootSignature = mRootSignature.Get();
		{
			D3D12_SHADER_BYTECODE	shader_bytecode;
			shader_bytecode.pShaderBytecode = vertex_shader->GetBufferPointer();
			shader_bytecode.BytecodeLength = vertex_shader->GetBufferSize();
			pso_desc.VS = shader_bytecode;
		}
		{
			D3D12_SHADER_BYTECODE	shader_bytecode;
			shader_bytecode.pShaderBytecode = pixel_shader->GetBufferPointer();
			shader_bytecode.BytecodeLength = pixel_shader->GetBufferSize();
			pso_desc.PS = shader_bytecode;
		}
		{
			D3D12_RASTERIZER_DESC	rasterizer_desc = {};
			rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
			rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
			rasterizer_desc.FrontCounterClockwise = false;
			rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			rasterizer_desc.DepthClipEnable = true;
			rasterizer_desc.MultisampleEnable = false;
			rasterizer_desc.AntialiasedLineEnable = false;
			rasterizer_desc.ForcedSampleCount = 0;
			rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
			pso_desc.RasterizerState = rasterizer_desc;
		}
		{
			D3D12_BLEND_DESC	blend_desc = {};
			blend_desc.AlphaToCoverageEnable = false;
			blend_desc.IndependentBlendEnable = false;
			for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
				blend_desc.RenderTarget[i].BlendEnable = true;
				blend_desc.RenderTarget[i].LogicOpEnable = false;
				blend_desc.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blend_desc.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				blend_desc.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
				blend_desc.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
				blend_desc.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
				blend_desc.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
				blend_desc.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
				blend_desc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			}
			pso_desc.BlendState = blend_desc;
		}
		pso_desc.DepthStencilState.DepthEnable = false;
		pso_desc.DepthStencilState.StencilEnable = false;
		pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pso_desc.SampleMask = UINT_MAX;
		pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pso_desc.NumRenderTargets = 1;
		pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pso_desc.SampleDesc.Count = 1;
		if (FAILED(mD3D12Device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&mPipelineState)))) {
			return E_FAIL;
		}
	}

	// コマンドリストを作成
	if (FAILED(mD3D12Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mCommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(mCommandList.GetAddressOf())))) {
		MessageBox(nullptr, L"コマンドリストの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	//Vertex vertices[] =
	//{
	//	{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},
	//	{{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
	//	{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
	//	{{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
	//};

	//// 頂点バッファサイズ
	//const UINT vertex_buffer_size = sizeof(vertices);

	//// コミットリソースを作成
	//{
	//	const CD3DX12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//	const CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(vertex_buffer_size);
	//	if (FAILED(mD3D12Device->CreateCommittedResource(
	//		&heap_properties,
	//		D3D12_HEAP_FLAG_NONE,
	//		&resource_desc,
	//		D3D12_RESOURCE_STATE_GENERIC_READ,
	//		nullptr,
	//		IID_PPV_ARGS(mVertexBuffer.GetAddressOf())))) {
	//		MessageBox(nullptr, L"コミットリソースの作成に失敗。", nullptr, MB_OK | MB_ICONERROR);
	//		return E_FAIL;
	//	}
	//}

	//// 頂点バッファに頂点データをコピー
	//UINT8* vertex_data_begin;
	//CD3DX12_RANGE read_range(0, 0);
	//mVertexBuffer->Map(0, &read_range, reinterpret_cast<void**>(&vertex_data_begin));
	//memcpy(vertex_data_begin, vertices, sizeof(vertices));
	//mVertexBuffer->Unmap(0, nullptr);

	//// 頂点バッファビューを初期化
	//mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	//mVertexBufferView.StrideInBytes = sizeof(Vertex);
	//mVertexBufferView.SizeInBytes = vertex_buffer_size;

	if (FAILED(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)))) {
		return E_FAIL;
	}
	mFenceValue = 1;

	// フレーム同期に使用するイベントハンドラを作成
	mFenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (mFenceEvent == nullptr) {
		if (FAILED(HRESULT_FROM_WIN32(GetLastError()))) {
			return E_FAIL;
		}
	}

	if (FAILED(WaitForPreviousFrame())) {
		return E_FAIL;
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 64;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(mD3D12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mSrvHeapForImgui)))){
			return E_FAIL;
		}
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(hWnd);

	ImGui_ImplDX12_Init(mD3D12Device.Get(), FRAME_COUNT,
		DXGI_FORMAT_R8G8B8A8_UNORM, mSrvHeapForImgui.Get(),
		mSrvHeapForImgui->GetCPUDescriptorHandleForHeapStart(),
		mSrvHeapForImgui->GetGPUDescriptorHandleForHeapStart());

	return S_OK;
}

HRESULT Device::RenderBegin()
{
	mCommandList->RSSetViewports(1, &mViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// バックバッファをレンダリングターゲットとして使用
	{
		D3D12_RESOURCE_BARRIER	resource_barrier = {};
		resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resource_barrier.Transition.pResource = mRenderTargets[mFrameIndex].Get();
		resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		mCommandList->ResourceBarrier(1, &resource_barrier);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRtvDescriptorSize);
	mCommandList->OMSetRenderTargets(1, &rtv_handle, false, nullptr);

	// バックバッファに描画
	const float	clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	mCommandList->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
	
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return S_OK;
}

HRESULT Device::RenderEnd()
{
	ImGui::Render();
	mCommandList->SetDescriptorHeaps(1, mSrvHeapForImgui.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());

	// バックバッファを表示
	{
		D3D12_RESOURCE_BARRIER	resource_barrier = {};
		resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resource_barrier.Transition.pResource = mRenderTargets[mFrameIndex].Get();
		resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		mCommandList->ResourceBarrier(1, &resource_barrier);
	}

	if (FAILED(mCommandList->Close())) {
		return E_FAIL;
	}

	// コマンドリストを実行
	ID3D12CommandList* command_lists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(command_lists), command_lists);

	// フレームを最終出力
	if (FAILED(mSwapChain->Present(1, 0))) {
		return E_FAIL;
	}

	if (FAILED(WaitForPreviousFrame())) {
		return E_FAIL;
	}

	if (FAILED(mCommandAllocator->Reset())) {
		return E_FAIL;
	}
	if (FAILED(mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Device::WaitForPreviousFrame()
{
	const UINT64 fence = mFenceValue;
	if (FAILED(mCommandQueue->Signal(mFence.Get(), fence))) {
		return E_FAIL;
	}
	++mFenceValue;

	// 前のフレームが終了するまで待機
	if (mFence->GetCompletedValue() < fence) {
		if (FAILED(mFence->SetEventOnCompletion(fence, mFenceEvent))) {
			return E_FAIL;
		}
		WaitForSingleObject(mFenceEvent, INFINITE);
	}

	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

	return S_OK;
}

HRESULT Device::Shutdown()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CloseHandle(mFenceEvent);
	return S_OK;
}