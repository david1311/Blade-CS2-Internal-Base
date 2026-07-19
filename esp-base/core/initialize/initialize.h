#pragma once

namespace core {
	class c_initialize {
	public:
		auto begin( ) -> void;
	};
	inline auto initialize = std::make_shared<c_initialize>( );

	class c_console {
	public:
		auto allocate( ) -> void;
	};
	inline auto console = std::make_shared<c_console>( );
}