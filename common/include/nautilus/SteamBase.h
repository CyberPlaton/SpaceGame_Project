#pragma once

#include"Base.h"

#include"steam/steam_api.h"
#include"steam/isteamfriends.h"
#include"steam/steam_gameserver.h"


extern "C" void _cdecl SteamAPIDebugMessage(int nSeverity, const char* pchDebugText);

namespace nautilus {

	namespace network {
		
		class SteamSystem {
		public:
			SteamSystem();
			~SteamSystem();



			bool init();


			// Enumarate friends of logged in user to console.
			void enumarateClientFriends();

			// Enable steam to put out debug messages.
			// This also makes assertion errors possible on severe errors.
			void enableDebugMessageHook() { SteamUtils()->SetWarningMessageHook(&SteamAPIDebugMessage); }



		private:



		private:
		};

	}


}

