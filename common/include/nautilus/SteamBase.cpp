#include"SteamBase.h"


extern "C" void _cdecl SteamAPIDebugMessage(int nSeverity, const char* pchDebugText)
{
	using namespace std;
	cout << color(colors::RED);

	cout << "Steam Debug Message: \n";

	cout << pchDebugText << white << endl;

	assert(nSeverity <= 1 && "Severe error in Steam occured!");
}

namespace nautilus {

	namespace network {


		bool SteamSystem::init() {

			if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid)) {

				// Close application.
				return false;
			}


			if (!SteamAPI_Init()) {
				return false;
			}


			using namespace std;
			cout << color(colors::GREEN);
			cout << "Connected to steam account: \"" << SteamFriends()->GetPersonaName() << "\"." << white << endl;


#ifdef _DEBUG
			enableDebugMessageHook();
#endif


			return true;
		}




		void SteamSystem::enumarateClientFriends() {

			for (int i = 0; i < SteamFriends()->GetFriendCount(k_EFriendFlagAll); i++) {

				CSteamID id = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagAll);

				using namespace std;
				cout << color(colors::MAGENTA);
				cout << "Friend: "<< i << ".) " << SteamFriends()->GetFriendPersonaName(id) << white << endl;
			
			}

		}




		SteamSystem::SteamSystem() {
			
		}



		SteamSystem::~SteamSystem() {

			SteamAPI_Shutdown();
		}
	}


}
