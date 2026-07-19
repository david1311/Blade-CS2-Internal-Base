#include "pch.h"

unsigned __stdcall main_thread( void* ) {
	core::initialize->begin( );
	return 0;
}

BOOL APIENTRY DllMain( HMODULE module , DWORD type , LPVOID ) {
	if ( type == DLL_PROCESS_ATTACH ) {
		DisableThreadLibraryCalls( module );
		_beginthreadex( nullptr , 0 , main_thread , nullptr , 0 , 0 );
	}
	return TRUE;
}
