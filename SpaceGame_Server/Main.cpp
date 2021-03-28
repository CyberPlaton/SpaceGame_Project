
#include"Main.h"


using namespace nautilus::network;


#define SERVER_PORT 7777

class SpaceGame_Server : public olc::net::server_interface<NetMsg> {
public:

	SpaceGame_Server() : olc::net::server_interface<NetMsg>(SERVER_PORT){

	}



	bool OnClientConnect(std::shared_ptr<olc::net::connection<NetMsg>> client) override {

		return true;
	}


	void OnClientValidated(std::shared_ptr<olc::net::connection<NetMsg>> client) override {

		using namespace olc;
		net::message<NetMsg> msg;

		msg.header.id = NetMsg::Client_Accepted;
		client->Send(msg);
	}


	void OnClientDisconnect(std::shared_ptr<olc::net::connection<NetMsg>> client) override {

		using namespace std;

		if (client) {

			if (m_PlayerLobby.find(client->GetID()) != m_PlayerLobby.end()) {

				auto& pd = m_PlayerLobby[client->GetID()];

				m_PlayerLobby.erase(client->GetID());
				m_GarbageIDs.push_back(client->GetID());
			}
		}
	}


	void OnMessage(std::shared_ptr<olc::net::connection<NetMsg>> client, olc::net::message<NetMsg>& msg) override {

		using namespace std;
		using namespace olc::net;
		using namespace nautilus::network;


		// Garbage collection.
		if (m_GarbageIDs.size() > 0) {

			for (auto it : m_GarbageIDs) {

				olc::net::message<NetMsg> message;
				message.header.id = NetMsg::Game_RemovePlayer;

				message << it;

				// Inform other players about removal of player.
				MessageAllClients(message);

			}


			m_GarbageIDs.clear();
		}





		if (msg.header.id == NetMsg::Client_RegisterWithServer) {

			// Store player in Lobby.
			PlayerDescription desc;
			msg >> desc;


			desc.m_PlayerNetworkID = client->GetID(); // Give him an ID.
			m_PlayerLobby.insert_or_assign(desc.m_PlayerNetworkID, desc);


			// Inform the client that server gave him an ID.
			message<NetMsg> messageSend;
			messageSend.header.id = NetMsg::Client_AssignID;
			messageSend << desc.m_PlayerNetworkID;
			MessageClient(client, messageSend);



			// Inform all clients that a Player was added.
			message<NetMsg> messageAddPlayer;
			messageAddPlayer.header.id = NetMsg::Game_AddPlayer;
			messageAddPlayer << desc;
			MessageAllClients(messageAddPlayer);


			// Inform new incomming player about all players currently in lobby.
			// We send for the client the instance that he will control too, in order
			// that he is ABLE to control an entity, that is alter its components etc.
			//
			for (const auto& player : m_PlayerLobby) {


				message<NetMsg> messageAddOtherPlayers;
				messageAddOtherPlayers.header.id = NetMsg::Game_AddPlayer;
				messageAddOtherPlayers << player.second;
				MessageClient(client, messageAddOtherPlayers);

			}
		}
		else if (msg.header.id == NetMsg::Client_UnregisterWithServer) {
			return;
		}
		else if (msg.header.id == NetMsg::Game_UpdatePlayer) {


			message<NetMsg> temp = msg;
			PlayerDescription desc;
			temp >> desc;

			using namespace std;
			cout << color(colors::GREEN);
			cout << "Player \"" << client->GetID() << "\" Position: (" << desc.m_PlayerPositionX <<  ":" << desc.m_PlayerPositionY << ")" << endl << endl;
			cout << "Player \"" << client->GetID() << "\" Velocity: (" << desc.m_PlayerVelocityX << ":" << desc.m_PlayerVelocityY << ")" << white << endl;

			// Just redirect the message to all clients.
			MessageAllClients(msg);
		}


	}


private:


	// Server has map of all entities.
	std::map<uint32_t, PlayerDescription> m_PlayerLobby;
	std::vector<uint32_t> m_GarbageIDs;


private:

};



int main() {

	using namespace std;

	SpaceGame_Server server;
	server.Start();

	cout << color(colors::YELLOW);
	cout << "Listening on: \"" << server.GetIpAddress() << "\":\"" << SERVER_PORT << "\". " << white << endl;
	

	while (1) {

		server.Update(-1, true);
	}

	server.Stop();

	return 0;
}
