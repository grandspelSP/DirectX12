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
#include "Imgui/imgui.h"

namespace {
	const wchar_t* WINDOW_CLASS{ _T("DirectX12Test") };
	const wchar_t* WINDOW_TITLE{ WINDOW_CLASS };
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#if DEBUG
void ShowUserGuide()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::BulletText("Double-click on title bar to collapse window.");
	ImGui::BulletText(
		"Click and drag on lower corner to resize window\n"
		"(double-click to auto fit window to its contents).");
	ImGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
	ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
	ImGui::BulletText("CTRL+Tab to select a window.");
	if (io.FontAllowUserScaling)
		ImGui::BulletText("CTRL+Mouse Wheel to zoom window contents.");
	ImGui::BulletText("While inputing text:\n");
	ImGui::Indent();
	ImGui::BulletText("CTRL+Left/Right to word jump.");
	ImGui::BulletText("CTRL+A or double-click to select all.");
	ImGui::BulletText("CTRL+X/C/V to use clipboard cut/copy/paste.");
	ImGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
	ImGui::BulletText("ESCAPE to revert.");
	ImGui::Unindent();
	ImGui::BulletText("With keyboard navigation enabled:");
	ImGui::Indent();
	ImGui::BulletText("Arrow keys to navigate.");
	ImGui::BulletText("Space to activate a widget.");
	ImGui::BulletText("Return to input text into a widget.");
	ImGui::BulletText("Escape to deactivate a widget, close popup, exit child window.");
	ImGui::BulletText("Alt to jump to the menu layer of a window.");
	ImGui::Unindent();
}
#endif // DEBUG


SampleImage image;
SampleImage image2;

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, nMsg, wParam, lParam))
		return true;

	switch (nMsg) {
	case WM_PAINT:
		// 描画
		GetDevice()->RenderBegin();

#if DEBUG
		ShowUserGuide();
#endif // DEBUG

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

		image.enter(1);
		image2.enter(2);

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
			image2.update();

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// 終了時の後処理
	GetDevice()->WaitForPreviousFrame();
	GetDevice()->Shutdown();
	SingletonFinalizer::finalize();

	return static_cast<int>(msg.wParam);
}

