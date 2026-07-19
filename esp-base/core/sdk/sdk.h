#pragma once

namespace offsets {
	constexpr std::uintptr_t dwEntityList = 0x254EE60;
	constexpr std::uintptr_t dwLocalPlayerPawn = 0x23A4238;
	constexpr std::uintptr_t dwViewMatrix = 0x23A9340;
	constexpr std::uintptr_t listEntry = 0x10;
	constexpr std::uintptr_t identityStride = 0x70;
	namespace C_BaseEntity {
		constexpr std::uintptr_t m_pGameSceneNode = 0x330;
		constexpr std::uintptr_t m_iHealth = 0x34C;
		constexpr std::uintptr_t m_lifeState = 0x354;
		constexpr std::uintptr_t m_iTeamNum = 0x3E7;
	}
	namespace CGameSceneNode {
		constexpr std::uintptr_t m_vecAbsOrigin = 0xC8;
		constexpr std::uintptr_t m_bDormant = 0x103;
	}
	namespace CCSPlayerController {
		constexpr std::uintptr_t m_hPlayerPawn = 0x914;
		constexpr std::uintptr_t m_sSanitizedPlayerName = 0x868;
	}
}

namespace SDK {
	struct Vector {
		float x {} , y {} , z {};
	};

	struct VMatrix {
		float m[ 4 ][ 4 ] {};
	};

	class CGameSceneNode {
	public:
		CGameSceneNode( ) = default;
		CGameSceneNode( std::uintptr_t address ) : address( address ) { }
		explicit operator bool( ) const { return address != 0; }
		auto m_vecAbsOrigin( ) const -> Vector {
			return *reinterpret_cast< Vector* >( address + offsets::CGameSceneNode::m_vecAbsOrigin );
		}
		auto m_bDormant( ) const -> bool {
			return *reinterpret_cast< bool* >( address + offsets::CGameSceneNode::m_bDormant );
		}
		std::uintptr_t address = 0;
	};

	class C_BaseEntity {
	public:
		C_BaseEntity( ) = default;
		C_BaseEntity( std::uintptr_t address ) : address( address ) { }
		explicit operator bool( ) const { return address != 0; }

		auto m_iHealth( ) const -> std::int32_t {
			return *reinterpret_cast< std::int32_t* >( address + offsets::C_BaseEntity::m_iHealth );
		}

		auto m_lifeState( ) const -> std::uint8_t {
			return *reinterpret_cast< std::uint8_t* >( address + offsets::C_BaseEntity::m_lifeState );
		}

		auto m_iTeamNum( ) const -> std::uint8_t {
			return *reinterpret_cast< std::uint8_t* >( address + offsets::C_BaseEntity::m_iTeamNum );
		}

		auto m_pGameSceneNode( ) const -> CGameSceneNode {
			return *reinterpret_cast< std::uintptr_t* >( address + offsets::C_BaseEntity::m_pGameSceneNode );
		}

		auto isAlive( ) const -> bool {
			return m_lifeState( ) == 0 && m_iHealth( ) > 0;
		}
		std::uintptr_t address = 0;
	};

	class C_CSPlayerPawn : public C_BaseEntity {
	public:
		using C_BaseEntity::C_BaseEntity;
	};

	class CCSPlayerController : public C_BaseEntity {
	public:
		using C_BaseEntity::C_BaseEntity;
		auto m_hPlayerPawn( ) const -> std::uint32_t {
			return *reinterpret_cast< std::uint32_t* >( address + offsets::CCSPlayerController::m_hPlayerPawn );
		}

		auto PawnIndex( ) const -> int {
			return static_cast< int >( m_hPlayerPawn( ) & 0x7FFF );
		}
	};
	class CGameEntitySystem {
	public:
		CGameEntitySystem( ) = default;
		CGameEntitySystem( std::uintptr_t address ) : address( address ) { }
		explicit operator bool( ) const { return address != 0; }

		auto Get( int index ) const -> std::uintptr_t {
			const auto entry = *reinterpret_cast< std::uintptr_t* >(
				address + offsets::listEntry + 8ull * ( static_cast< std::uint32_t >( index ) >> 9 ) );
			if ( !entry )
				return 0;
			return *reinterpret_cast< std::uintptr_t* >(
				entry + offsets::identityStride * ( index & 0x1FF ) );
		}

		auto GetController( int index ) const -> CCSPlayerController {
			return Get( index );
		}
		auto GetPawn( int index ) const -> C_CSPlayerPawn {
			return Get( index );
		}
		auto GetPawn( const CCSPlayerController& controller ) const -> C_CSPlayerPawn {
			return Get( controller.PawnIndex( ) );
		}
		std::uintptr_t address = 0;
	};

	class CSGOInput {
	public:
		static auto ClientModule( ) -> std::uintptr_t {
			return reinterpret_cast< std::uintptr_t >( GetModuleHandleA( "client.dll" ) );
		}
		static auto EntitySystem( ) -> CGameEntitySystem {
			return *reinterpret_cast< std::uintptr_t* >( ClientModule( ) + offsets::dwEntityList );
		}
		static auto LocalPawn( ) -> C_CSPlayerPawn {
			return *reinterpret_cast< std::uintptr_t* >( ClientModule( ) + offsets::dwLocalPlayerPawn );
		}
		static auto ViewMatrix( ) -> VMatrix {
			return *reinterpret_cast< VMatrix* >( ClientModule( ) + offsets::dwViewMatrix );
		}
	};
}