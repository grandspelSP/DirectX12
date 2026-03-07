//=========================================================
// @file Main.cpp
// @auth Kota Tamaoki
// @date 2022/12/03
//=========================================================

#include "Singleton.h"
#include "ActorBase.h"
#include "SampleImage.h"
#include "Component/TextureRender.h"
#include "Device.h"

namespace {
	const wchar_t* WINDOW_CLASS{ _T("DirectX12Test") };
	const wchar_t* WINDOW_TITLE{ WINDOW_CLASS };
}

SampleImage image;
SampleImage image2;

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) {
	case WM_PAINT:
		// 描画
		GetDevice()->RenderBegin();

		image.draw();
		image2.draw();

		GetDevice()->RenderEnd();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, nMsg, wParam, lParam);
	}

	return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, TCHAR* lpszCmdLine, int nCmdShow)
{
	// ウィンドウを作成
	{
		WNDCLASSEX	window_class = {};

		// ウィンドウクラスを登録
		window_class.cbSize = sizeof(WNDCLASSEX);
		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		window_class.lpfnWndProc = WindowProc;
		window_class.hInstance = hInstance;
		window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		window_class.lpszClassName = WINDOW_CLASS;
		RegisterClassEx(&window_class);
	}

	{
		RECT	window_rect = { 0, 0, 1280, 720 };
		AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, false);

		HWND	h_wnd = CreateWindow(
			WINDOW_CLASS,
			WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			window_rect.right - window_rect.left,
			window_rect.bottom - window_rect.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr);
		// DirectXを初期化
		if (FAILED(GetDevice()->InitDevice(h_wnd))) {
			MessageBox(h_wnd, _T("DirectXの初期化が失敗しました"), _T("Init"), MB_OK | MB_ICONEXCLAMATION);
			return 0;
		}

		image.enter();
		image2.enter();

		ShowWindow(h_wnd, SW_SHOW);
		UpdateWindow(h_wnd);
	}

	// メッセージループ
	MSG	msg;

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}

			image.update();

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// 終了時の後処理
	GetDevice()->WaitForPreviousFrame();
	CloseHandle(GetDevice()->mFenceEvent);
	SingletonFinalizer::finalize();

	return static_cast<int>(msg.wParam);
}