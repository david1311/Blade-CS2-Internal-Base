#pragma once

struct Storage_t {
	struct {
		bool Masterswitch { false };
		bool Boxes { false };
		int BoxType { 0 };
	} ESP;
};
inline auto Storage = std::make_shared<Storage_t>( );