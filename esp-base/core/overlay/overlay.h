#pragma once

#include <d3d11.h>
#include <dxgi.h>

struct ImFont;
struct ImFontAtlas;

namespace Overlay {
	class CMenu {
	public:
		bool Open = false;
		bool ShowDemo = false;
		bool ShowWatermark = true;
		ImFont* Font = nullptr;

		auto Begin( ) -> void;
		auto Render( ) -> void;

	private:
		bool Hooked = false;
		bool Initialized = false;

		HWND hWnd = nullptr;
		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pDeviceContext = nullptr;
		ID3D11RenderTargetView* pRenderTargetView = nullptr;
		WNDPROC wndProc = nullptr;

		auto CreateRenderTarget( IDXGISwapChain* Swap ) -> void;
		auto CleanupRenderTarget( ) -> void;
		auto RegisterFonts( ImFontAtlas* Atlas ) -> void;
		auto Init( IDXGISwapChain* Swap ) -> bool;
		auto OnPresent( IDXGISwapChain* Swap ) -> void;
		auto OnResize( ) -> void;

		static auto InstallHooks( ) -> bool;
		static auto hkPresent( IDXGISwapChain* Swap , UINT Sync , UINT Flags ) -> HRESULT;
		static auto hkResizeBuffers( IDXGISwapChain* Swap , UINT Count , UINT Width , UINT Height , DXGI_FORMAT Format , UINT Flags ) -> HRESULT;
		static auto hkWndProc( HWND hWnd , UINT Msg , WPARAM wParam , LPARAM lParam ) -> LRESULT;
	};

	inline auto Menu = std::make_shared<CMenu>( );
}
