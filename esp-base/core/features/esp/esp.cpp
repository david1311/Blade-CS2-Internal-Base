#include "esp.h"

#include <core/sdk/sdk.h>
#include <extern/imgui/imgui.h>

namespace Helper {
	auto WorldToScreen( const SDK::VMatrix& vm , const SDK::Vector& world , float screen_w , float screen_h , SDK::Vector& out ) -> bool {
		const float w = vm.m[ 3 ][ 0 ] * world.x + vm.m[ 3 ][ 1 ] * world.y + vm.m[ 3 ][ 2 ] * world.z + vm.m[ 3 ][ 3 ];
		if ( w < 0.001f )
			return false;

		const float inv_w = 1.f / w;
		const float x = ( vm.m[ 0 ][ 0 ] * world.x + vm.m[ 0 ][ 1 ] * world.y + vm.m[ 0 ][ 2 ] * world.z + vm.m[ 0 ][ 3 ] ) * inv_w;
		const float y = ( vm.m[ 1 ][ 0 ] * world.x + vm.m[ 1 ][ 1 ] * world.y + vm.m[ 1 ][ 2 ] * world.z + vm.m[ 1 ][ 3 ] ) * inv_w;

		out.x = screen_w * 0.5f + ( 0.5f * x * screen_w );
		out.y = screen_h * 0.5f - ( 0.5f * y * screen_h );
		out.z = 0.f;
		return true;
	}
}

auto ESP_t::Render( ) -> void {
	if ( !Storage->ESP.Masterswitch )
		return;

	const auto LocalPawn = SDK::CSGOInput::LocalPawn( );
	const auto Entities = SDK::CSGOInput::EntitySystem( );
	const auto ViewMatrix = SDK::CSGOInput::ViewMatrix( );

	if ( !LocalPawn )
		return;
	
	if ( !Entities )
		return;

	const ImVec2 display = ImGui::GetIO( ).DisplaySize;
	const float SW = display.x;
	const float SH = display.y;
	auto* draw = ImGui::GetBackgroundDrawList( );

	for ( int i = 1; i <= 64; ++i ) {
		const auto Controller = Entities.GetController( i );
		if ( !Controller )
			continue;

		const auto Pawn = Entities.GetPawn( Controller );
		if ( !Pawn || Pawn.address == LocalPawn.address )
			continue;

		if ( !Pawn.isAlive( ) || Pawn.m_iTeamNum( ) == LocalPawn.m_iTeamNum( ) )
			continue;

		const auto Node = Pawn.m_pGameSceneNode( );
		if ( !Node || Node.m_bDormant( ) )
			continue;

		SDK::Vector Feet = Node.m_vecAbsOrigin( );
		SDK::Vector Head = { Feet.x, Feet.y, Feet.z + 72.f };

		SDK::Vector Feet2D {} , Head2D {};
		if ( !Helper::WorldToScreen( ViewMatrix , Feet , SW , SH , Feet2D ) )
			continue;

		if ( !Helper::WorldToScreen( ViewMatrix , Head , SW , SH , Head2D ) )
			continue;

		float Height = Feet2D.y - Head2D.y;
		if ( Height <= 1.f )
			continue; // size btw if you don't know

		float Width = Height * 0.55f;
		float X = Head2D.x - Width * 0.5f;
		float Y = Head2D.y;

		ImVec2 pMin { X, Y };
		ImVec2 pSize { Width, Height };
		ImVec2 pMax { pMin.x + pSize.x, pMin.y + pSize.y };

		if ( Storage->ESP.Boxes ) {
			draw->AddRect( pMin , pMax , IM_COL32_WHITE );
		}
	}
}
