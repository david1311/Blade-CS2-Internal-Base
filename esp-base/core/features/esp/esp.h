#pragma once

struct ESP_t {
	auto Render( ) -> void;
};
inline auto ESP = std::make_shared<ESP_t>( );