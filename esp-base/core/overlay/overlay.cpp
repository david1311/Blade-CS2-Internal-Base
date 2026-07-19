#include "pch.h"
#include "overlay.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>

#include <core/features/esp/esp.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam );

namespace Overlay {
	namespace {
		using present_fn = HRESULT( __stdcall* )( IDXGISwapChain* , UINT , UINT );
		using resize_buffers_fn = HRESULT( __stdcall* )( IDXGISwapChain* , UINT , UINT , UINT , DXGI_FORMAT , UINT );

		present_fn o_present = nullptr;
		resize_buffers_fn o_resize_buffers = nullptr;

		bool esp_enabled = false;
		bool aimbot_enabled = false;
	}

	auto CMenu::CreateRenderTarget( IDXGISwapChain* Swap ) -> void {
		CleanupRenderTarget( );
		ID3D11Texture2D* back_buffer = nullptr;
		if ( FAILED( Swap->GetBuffer( 0 , IID_PPV_ARGS( &back_buffer ) ) ) || !back_buffer )
			return;
		pDevice->CreateRenderTargetView( back_buffer , nullptr , &pRenderTargetView );
		back_buffer->Release( );
	}

	auto CMenu::CleanupRenderTarget( ) -> void {
		if ( pRenderTargetView ) {
			pRenderTargetView->Release( );
			pRenderTargetView = nullptr;
		}
	}

	auto CMenu::RegisterFonts( ImFontAtlas* Atlas ) -> void {
		if ( !Atlas )
			return;

		ImFontConfig cfg{};
		cfg.OversampleH = 1;
		cfg.OversampleV = 1;
		cfg.PixelSnapH = true;
		cfg.SizePixels = 13.f;

		Font = Atlas->AddFontFromFileTTF( "E:\\Windows\\Fonts\\tahoma.ttf" , 13.f , &cfg , Atlas->GetGlyphRangesCyrillic( ) );
		if ( !Font ) {
			Font = Atlas->AddFontDefault( &cfg );
			logger->warn( "overlay -> tahoma missing, default font" );
		}
	}

	auto CMenu::Init( IDXGISwapChain* Swap ) -> bool {
		if ( Initialized )
			return true;

		DXGI_SWAP_CHAIN_DESC desc{};
		if ( FAILED( Swap->GetDesc( &desc ) ) )
			return false;

		hWnd = desc.OutputWindow;
		if ( !hWnd )
			return false;

		if ( FAILED( Swap->GetDevice( IID_PPV_ARGS( &pDevice ) ) ) || !pDevice )
			return false;

		pDevice->GetImmediateContext( &pDeviceContext );
		if ( !pDeviceContext )
			return false;

		CreateRenderTarget( Swap );

		IMGUI_CHECKVERSION( );
		ImGui::CreateContext( );
		ImGuiIO& io = ImGui::GetIO( );
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;

		RegisterFonts( io.Fonts );
		if ( Font )
			io.FontDefault = Font;

		ImGui::StyleColorsDark( );

		ImGui_ImplWin32_Init( hWnd );
		ImGui_ImplDX11_Init( pDevice , pDeviceContext );

		wndProc = reinterpret_cast<WNDPROC>( SetWindowLongPtrW( hWnd , GWLP_WNDPROC , reinterpret_cast<LONG_PTR>( &CMenu::hkWndProc ) ) );
		Initialized = true;
		logger->info( "overlay -> imgui ready" );
		return true;
	}

	auto CMenu::OnResize( ) -> void {
		if ( !Initialized )
			return;

		CleanupRenderTarget( );
		ImGui_ImplDX11_InvalidateDeviceObjects( );
	}

	auto CMenu::Render( ) -> void {
		if ( GetAsyncKeyState( VK_INSERT ) & 1 )
			Open = !Open;

		if ( ShowWatermark ) {
			ImGui::SetNextWindowPos( ImVec2( 12.f , 12.f ) , ImGuiCond_Always );
			ImGui::SetNextWindowBgAlpha( 0.35f );
			ImGui::Begin( "##watermark" , nullptr ,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove );
			ImGui::TextUnformatted( "esp-base" );
			ImGui::End( );
		}

		if ( !Open )
			return;

		ImGui::SetNextWindowSize( ImVec2( 420.f , 280.f ) , ImGuiCond_FirstUseEver );
		ImGui::Begin( "Blade CS2 Base" , &Open );
		ImGui::Checkbox( "Masterswitch" , &Storage->ESP.Masterswitch );
		ImGui::Checkbox( "Boxes" , &Storage->ESP.Boxes );

		ImGui::End( );

		if ( ShowDemo )
			ImGui::ShowDemoWindow( &ShowDemo );
	}

	auto CMenu::OnPresent( IDXGISwapChain* Swap ) -> void {
		if ( !Initialized && !Init( Swap ) )
			return;

		if ( !pRenderTargetView )
			CreateRenderTarget( Swap );

		ImGui_ImplDX11_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );

		if ( Font )
			ImGui::PushFont( Font );

		Render( );
		ESP->Render( );

		if ( Font )
			ImGui::PopFont( );

		ImGui::Render( );
		pDeviceContext->OMSetRenderTargets( 1 , &pRenderTargetView , nullptr );
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
	}

	auto CMenu::hkPresent( IDXGISwapChain* Swap , UINT Sync , UINT Flags ) -> HRESULT {
		Menu->OnPresent( Swap );
		return o_present( Swap , Sync , Flags );
	}

	auto CMenu::hkResizeBuffers( IDXGISwapChain* Swap , UINT Count , UINT Width , UINT Height , DXGI_FORMAT Format , UINT Flags ) -> HRESULT {
		Menu->OnResize( );
		const auto result = o_resize_buffers( Swap , Count , Width , Height , Format , Flags );
		if ( SUCCEEDED( result ) )
			Menu->CreateRenderTarget( Swap );
		return result;
	}

	auto CMenu::hkWndProc( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam ) -> LRESULT {
		if ( Menu->Initialized && ImGui_ImplWin32_WndProcHandler( hwnd , msg , wparam , lparam ) )
			return true;

		if ( Menu->Initialized && Menu->Open ) {
			const ImGuiIO& io = ImGui::GetIO( );
			if ( io.WantCaptureMouse || io.WantCaptureKeyboard ) {
				switch ( msg ) {
				case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
				case WM_MOUSEWHEEL: case WM_MOUSEHWHEEL:
				case WM_KEYDOWN: case WM_KEYUP: case WM_SYSKEYDOWN: case WM_SYSKEYUP: case WM_CHAR:
					return true;
				default:
					break;
				}
			}
		}

		return CallWindowProcW( Menu->wndProc , hwnd , msg , wparam , lparam );
	}

	auto CMenu::InstallHooks( ) -> bool {
		WNDCLASSEXA wc{};
		wc.cbSize = sizeof( wc );
		wc.style = CS_CLASSDC;
		wc.lpfnWndProc = DefWindowProcA;
		wc.hInstance = GetModuleHandleA( nullptr );
		wc.lpszClassName = "esp_base_dx_tmp";

		if ( !RegisterClassExA( &wc ) )
			return false;

		HWND hwnd = CreateWindowA( wc.lpszClassName , "" , WS_OVERLAPPEDWINDOW , 0 , 0 , 100 , 100 , nullptr , nullptr , wc.hInstance , nullptr );
		if ( !hwnd ) {
			UnregisterClassA( wc.lpszClassName , wc.hInstance );
			return false;
		}

		DXGI_SWAP_CHAIN_DESC sd{};
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL level{};
		const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 , D3D_FEATURE_LEVEL_10_0 };
		IDXGISwapChain* swap = nullptr;
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;

		const HRESULT hr = D3D11CreateDeviceAndSwapChain( nullptr , D3D_DRIVER_TYPE_HARDWARE , nullptr , 0 , levels , 2 ,
			D3D11_SDK_VERSION , &sd , &swap , &device , &level , &context );
		if ( FAILED( hr ) || !swap ) {
			DestroyWindow( hwnd );
			UnregisterClassA( wc.lpszClassName , wc.hInstance );
			return false;
		}

		void** vtable = *reinterpret_cast<void***>( swap );
		o_present = reinterpret_cast<present_fn>( vtable[8] );
		o_resize_buffers = reinterpret_cast<resize_buffers_fn>( vtable[13] );

		DWORD old_protect = 0;
		VirtualProtect( &vtable[8] , sizeof( void* ) * 6 , PAGE_EXECUTE_READWRITE , &old_protect );
		vtable[8] = reinterpret_cast<void*>( &CMenu::hkPresent );
		vtable[13] = reinterpret_cast<void*>( &CMenu::hkResizeBuffers );
		VirtualProtect( &vtable[8] , sizeof( void* ) * 6 , old_protect , &old_protect );

		context->Release( );
		device->Release( );
		swap->Release( );
		DestroyWindow( hwnd );
		UnregisterClassA( wc.lpszClassName , wc.hInstance );
		return o_present && o_resize_buffers;
	}

	auto CMenu::Begin( ) -> void {
		if ( Hooked )
			return;

		if ( !InstallHooks( ) ) {
			logger->warn( "overlay -> failed to hook present" );
			return;
		}

		Hooked = true;
		logger->info( "overlay -> present hooked" );
	}
}
