#pragma once

#include"Base.h"
#include"SteamBase.h"

namespace nautilus {

	namespace network {



		enum class NetMsg : uint32_t {

			Server_GetStatus,
			Server_GetPing,


			Client_Accepted,
			Client_AssignID,
			Client_RegisterWithServer,
			Client_UnregisterWithServer,


			Game_AddPlayer,
			Game_RemovePlayer,
			Game_UpdatePlayer,
		};



		struct PlayerDescription {

			// Unique network id assigned by the server for
			// the client/player instance.
			//
			uint32_t m_PlayerNetworkID = 0;


			// Some special variables for the spacewar game.
			//
			// These are different for each game and MUST be adjusted for each accordingly.
			//
			uint32_t m_PlayerHealth = 0;
			float m_PlayerPositionX = 0.0f;
			float m_PlayerPositionY = 0.0f;
			uint32_t m_PlayerArmor = 0;
			float m_PlayerVelocityX = 0.0f;
			float m_PlayerVelocityY = 0.0f;
			float m_PlayerRotation = 0.0f;



			// Name of the players ship to be displayed.
			//
			// It should be possible to load dynamically a ship into the scene based on the name.
			//
			enum class PlayerRepresentation {
				Invalid = -1,
				Fighter = 0,
				Juggernaut,
				Raider,
				Interceptor
			};

			PlayerRepresentation m_PlayerShip = PlayerRepresentation::Invalid;

		};

	}

}
