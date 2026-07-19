#include "pch.h"
#include "initialize.h"

namespace core {
	auto c_console::allocate( ) -> void {
		AllocConsole( );
		FILE* file;

		freopen_s( &file , "CONIN$" , "r" , stdin );
		freopen_s( &file , "CONIN$" , "r" , stderr );
		freopen_s( &file , "CONOUT$" , "w" , stdout );

		SetConsoleTitleA( "esp-base - console" );
		HANDLE console_handle = GetStdHandle( STD_OUTPUT_HANDLE );
		DWORD mode = 0;
		if ( !GetConsoleMode( console_handle , &mode ) )
			return;

		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode( console_handle , mode );
	}

	auto c_initialize::begin( ) -> void {
		console->allocate( );

		logger->info( "esp-base loaded" );
		Beep( 500 , 500 );

		Overlay::Menu->Begin( );
	}
}
