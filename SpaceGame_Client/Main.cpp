#include"Main.h"

#define SERVER_PORT 7777

void SpaceGame_Client::onUpdate(float dt) {

	using namespace std;
	using namespace nautilus::network;
	using namespace olc::net;


	if (IsConnected()) {

		cout << color(colors::YELLOW);
		cout << "Network update..." << white << endl;

		uint32_t id = 0;


		// We have connected. So start to count the network latency.
		m_Timer->startTimer();


		while (!Incoming().empty()) {

			auto msg = Incoming().pop_front().msg;



			// Store how long we needed to get the next message, aka Ping.
			float ping = (float)m_Timer->getElapsedTime();
			cout << color(colors::RED);
			cout << "Ping: " << ping << white << endl;
			m_LastPing = ping;
			m_PingQueue.push_back(ping);
			m_Timer->startTimer();


			if (msg.header.id == NetMsg::Client_Accepted) {

				cout << color(colors::DARKMAGENTA);
				cout << "Network Message: \"Client_Accepted\"" << endl;

				// Register ourselves in the server,
				// by sending him our own player description.
				//
				message<NetMsg> outMsg;
				outMsg.header.id = NetMsg::Client_RegisterWithServer;

				// Send initial data of our space ship to the server.
				int ship = 0;
				cout << color(colors::WHITE);
				cout << "Choose your ship: 0 - 3" << white << endl;
				cin >> ship;
				if (ship == 0 || ship > 3) {

					m_PlayerDesc.m_PlayerShip = PlayerDescription::PlayerRepresentation::Fighter;
					m_PlayerDesc.m_PlayerArmor = 10;
					m_PlayerDesc.m_PlayerHealth = 10;
					m_PlayerDesc.m_PlayerPositionX = 1.0f;
					m_PlayerDesc.m_PlayerPositionY = 1.0f;
					m_PlayerDesc.m_PlayerRotation = 0.0f;
				}
				else if (ship == 1) {

					m_PlayerDesc.m_PlayerShip = PlayerDescription::PlayerRepresentation::Juggernaut;
					m_PlayerDesc.m_PlayerArmor = 35;
					m_PlayerDesc.m_PlayerHealth = 15;
					m_PlayerDesc.m_PlayerPositionX = 1.0f;
					m_PlayerDesc.m_PlayerPositionY = 1.0f;
					m_PlayerDesc.m_PlayerRotation = 0.0f;

				}
				else if (ship == 2) {

					m_PlayerDesc.m_PlayerShip = PlayerDescription::PlayerRepresentation::Raider;
					m_PlayerDesc.m_PlayerArmor = 15;
					m_PlayerDesc.m_PlayerHealth = 15;
					m_PlayerDesc.m_PlayerPositionX = 1.0f;
					m_PlayerDesc.m_PlayerPositionY = 1.0f;
					m_PlayerDesc.m_PlayerRotation = 0.0f;

				}
				else if (ship == 3) {

					m_PlayerDesc.m_PlayerShip = PlayerDescription::PlayerRepresentation::Interceptor;
					m_PlayerDesc.m_PlayerArmor = 5;
					m_PlayerDesc.m_PlayerHealth = 10;
					m_PlayerDesc.m_PlayerPositionX = 1.0f;
					m_PlayerDesc.m_PlayerPositionY = 1.0f;
					m_PlayerDesc.m_PlayerRotation = 0.0f;

				}

				outMsg << m_PlayerDesc;
				Send(outMsg);
			}


			else if (msg.header.id == NetMsg::Client_AssignID) {

				cout << color(colors::DARKMAGENTA);
				cout << "Network Message: \"Client_AssignID\"" << endl;

				msg >> id;
				m_PlayerID = id; // Server assigned us our Network GUID.
			}


			else if (msg.header.id == NetMsg::Game_AddPlayer) {

				cout << color(colors::DARKMAGENTA);
				cout << "Network Message: \"Game_AddPlayer\"" << endl;

				// Update or insert a player in the lobby.
				//
				PlayerDescription desc;
				msg >> desc;
				m_PlayerLobby.insert_or_assign(desc.m_PlayerNetworkID, desc);


				if (desc.m_PlayerNetworkID == m_PlayerID)
				{

					// Now we exist in game world
					m_WaitingForConnection = false;
				}


				// Create a representation for 
				// new added player.
				if (desc.m_PlayerShip == PlayerDescription::PlayerRepresentation::Fighter) {

					// Populate current scene with object...
					std::string network_id = std::to_string(desc.m_PlayerNetworkID);
					if (m_SceneManager->populateActiveScene("Ship_" + network_id, "ship_wasp_class.png")) {

					}
				}
				else if (desc.m_PlayerShip == PlayerDescription::PlayerRepresentation::Juggernaut) {

					// Populate current scene with object...
					std::string network_id = std::to_string(desc.m_PlayerNetworkID);
					if (m_SceneManager->populateActiveScene("Ship_" + network_id, "spaceShips_005.png")) {

					}
				}
				else if (desc.m_PlayerShip == PlayerDescription::PlayerRepresentation::Raider) {

					// Populate current scene with object...
					std::string network_id = std::to_string(desc.m_PlayerNetworkID);
					if (m_SceneManager->populateActiveScene("Ship_" + network_id, "spaceShips_002.png")) {

					}
				}
				else if (desc.m_PlayerShip == PlayerDescription::PlayerRepresentation::Interceptor) {

					// Populate current scene with object...
					std::string network_id = std::to_string(desc.m_PlayerNetworkID);
					if (m_SceneManager->populateActiveScene("Ship_" + network_id, "enemyRed3.png")) {

					}
				}
			}


			else if (msg.header.id == NetMsg::Game_RemovePlayer) {

				cout << color(colors::DARKMAGENTA);
				cout << "Network Message: \"Game_RemovePlayer\"" << endl;

				msg >> id;
				m_PlayerLobby.erase(id);
			}


			else if (msg.header.id == NetMsg::Game_UpdatePlayer) {

				cout << color(colors::DARKMAGENTA);
				cout << "Network Message: \"Game_UpdatePlayer\"" << endl;

				PlayerDescription desc;
				msg >> desc;

				// Try to avoid Bug.
				// We dont want to update own client player entity.
				//
				if (desc.m_PlayerNetworkID == m_PlayerID) break;


				m_PlayerLobby.insert_or_assign(desc.m_PlayerNetworkID, desc);
			}

		}
	}



	if (m_WaitingForConnection) {
		return;
	}


	// Update the clients ship position etc. based on its networked entity.
	using namespace nautilus::graphics;
	using namespace nautilus;



	// We have to check for input here, 
	// as if we updated our own client before this point and did not press any button
	// it will just stay unnoticed IF WE PRESS any button, because it will be nulled out
	// by the server update, which says we havent pressed anything/ changed velocity...
	//
	// THIS IS A BUG! Obviously.
	//
	// So how can we detect input change if we cannot do it here but have to rely on "EventManager" and "InputHandler" in
	// theyre own separate threads?
	//
	// Well, one possible solution is not to let the server update ourselves.
	// As we update everything here and send the server the data for updating,
	// is seems an overhead to try to update the data we have in place...
	//


	// Do the device input update.
	using namespace nautilus::hid;
	int frames = 2;

	// Here we check for gamepad, because we know that a gamepad is connected.
	// If we want to register same input for keys, then we have to check the values and
	// apply our velocity change input...
	//

	float x_axis_dt = m_HIDManager->getButtonValue(Button::Move_Left_Float);
	float y_axis_dt = m_HIDManager->getButtonValue(Button::Move_Up_Float);
	float rotation = m_HIDManager->getButtonValue(Button::Turn_Left_Float);

	if (x_axis_dt != 0.0f) {

		m_PlayerLobby[m_PlayerID].m_PlayerVelocityX += x_axis_dt; // the delta value is positive if we press stick to rght, else negative

	}
	if (y_axis_dt != 0.0f) {

		m_PlayerLobby[m_PlayerID].m_PlayerVelocityY += y_axis_dt; // same here, values are plus or minus...

	}
	if (rotation != 0.0f) {

		m_PlayerLobby[m_PlayerID].m_PlayerRotation -= rotation / 3.141f;
	}

	if (m_HIDManager->isButtonPressed(Button::Confirm)) {
		
		m_PlayerLobby[m_PlayerID].m_PlayerRotation += (1.5708f * 2);
	}

	if (m_HIDManager->isButtonHeld(Button::Move_Down)) {

		m_PlayerLobby[m_PlayerID].m_PlayerVelocityY -= 1.0f;

	}
	if (m_HIDManager->isButtonHeld(Button::Move_Up)) {

		m_PlayerLobby[m_PlayerID].m_PlayerVelocityY += 1.0f;
	}
	if (m_HIDManager->isButtonHeld(Button::Move_Right)) {

		m_PlayerLobby[m_PlayerID].m_PlayerVelocityX += 1.0f;

	}
	if (m_HIDManager->isButtonHeld(Button::Move_Left)) {

		m_PlayerLobby[m_PlayerID].m_PlayerVelocityX -= 1.0f;

	}
	if (m_HIDManager->isButtonHeld(Button::Turn_Right)) {

		m_PlayerLobby[m_PlayerID].m_PlayerRotation -= 0.3f;
	}
	if (m_HIDManager->isButtonHeld(Button::Turn_Left)) {

		m_PlayerLobby[m_PlayerID].m_PlayerRotation += 0.3f;
	}




	// Update the positions and data of the ships on the client side,
	// according to those on the server side.
	//
	// Iterate over every ship in the Lobby.
	//
	for (auto& it : m_PlayerLobby) {

		using namespace std;
		cout << color(colors::CYAN);
		cout << "Client Rendering Update: " << it.first << endl;
		//if (m_PlayerID == it.first) {
		//	cout << "Skipping local client player..." << white << endl;
		//	continue;
		//}


		// Get the associated entity with the iterated networked player.
		//
		std::string handle = "Ship_" + std::to_string(it.first);
		Ref<CEntity> entity = m_SceneManager->getSceneEntity(handle);


		// Dead Reckoning.
		// Try to predict the movement of the player.
		// 
		float dt = ImGui::GetIO().DeltaTime; // How long did we need for one frame, that is from last time we were here to now.
		float latency = 1 / 100.0f; // Simulate networking latency with constant 100ms.

		float potentialX = it.second.m_PlayerPositionX + it.second.m_PlayerVelocityX * dt;
		float potentialY = it.second.m_PlayerPositionY + it.second.m_PlayerVelocityY * dt;


		// Rendering position.
		// position for rendering as defined in networked position
		auto& transform = entity->getComponent<ComponentTransform>();
		transform.m_Position = glm::vec2(potentialX, potentialY);
		transform.m_Rotation = it.second.m_PlayerRotation;




		// Network position.
		//
		// We want to update the position only for our client,
		// because other clients will do the same in theyre instance.
		//
		if (m_PlayerID == it.first) {

			it.second.m_PlayerPositionX = potentialX;
			it.second.m_PlayerPositionY = potentialY;
		}
	}




	// Send player description
	// for this client instance.
	//
	// Other client instances will do the same.
	//
	PlayerDescription desc;
	desc.m_PlayerArmor = m_PlayerLobby[m_PlayerID].m_PlayerArmor;
	desc.m_PlayerHealth = m_PlayerLobby[m_PlayerID].m_PlayerHealth;
	desc.m_PlayerPositionX = m_PlayerLobby[m_PlayerID].m_PlayerPositionX;
	desc.m_PlayerPositionY = m_PlayerLobby[m_PlayerID].m_PlayerPositionY;
	desc.m_PlayerNetworkID = m_PlayerLobby[m_PlayerID].m_PlayerNetworkID;
	desc.m_PlayerShip = m_PlayerLobby[m_PlayerID].m_PlayerShip;
	desc.m_PlayerVelocityX = m_PlayerLobby[m_PlayerID].m_PlayerVelocityX;
	desc.m_PlayerVelocityY = m_PlayerLobby[m_PlayerID].m_PlayerVelocityY;
	desc.m_PlayerRotation = m_PlayerLobby[m_PlayerID].m_PlayerRotation;


	cout << color(colors::CYAN);
	cout << "Ship Rotation: " << desc.m_PlayerRotation << white << endl;

	olc::net::message<NetMsg> msg;
	msg.header.id = NetMsg::Game_UpdatePlayer;
	msg << desc;
	Send(msg);



	// Normalize velocity of player...
	if (m_PlayerLobby[m_PlayerID].m_PlayerVelocityX >= 0.0f) {
		m_PlayerLobby[m_PlayerID].m_PlayerVelocityX = (m_PlayerLobby[m_PlayerID].m_PlayerVelocityX - 0.3f <= 0.0f) ? 0.0f : m_PlayerLobby[m_PlayerID].m_PlayerVelocityX - 0.3f;
	}
	else {
		m_PlayerLobby[m_PlayerID].m_PlayerVelocityX = (m_PlayerLobby[m_PlayerID].m_PlayerVelocityX + 0.3f >= 0.0f) ? 0.0f : m_PlayerLobby[m_PlayerID].m_PlayerVelocityX + 0.3f;
	}
	if (m_PlayerLobby[m_PlayerID].m_PlayerVelocityY >= 0.0f) {
		m_PlayerLobby[m_PlayerID].m_PlayerVelocityY = (m_PlayerLobby[m_PlayerID].m_PlayerVelocityY - 0.3f <= 0.0f) ? 0.0f : m_PlayerLobby[m_PlayerID].m_PlayerVelocityY - 0.3f;
	}
	else {
		m_PlayerLobby[m_PlayerID].m_PlayerVelocityY = (m_PlayerLobby[m_PlayerID].m_PlayerVelocityY + 0.3f >= 0.0f) ? 0.0f : m_PlayerLobby[m_PlayerID].m_PlayerVelocityY + 0.3f;

	}

}





void SpaceGame_Client::onInit() {
	using namespace std;

	if (!Connect("127.0.0.1", SERVER_PORT)) {

		cout << color(colors::RED);
		cout << "Connection failed." << white << endl;
		return;
	}
	else {

		cout << color(colors::GREEN);
		cout << "Connection successful." << white << endl;

	}



	setFramerateGoverning(60);

}










int main(){

	using namespace nautilus::graphics;

	nautilus::graphics::WindowProperties* props = new nautilus::graphics::WindowProperties();
	props->m_ClearColor = { 0.01f, 0.2f, 0.4f, 1.0f };
	props->m_CursorDisabled = false;
	props->m_Fullscreen = false;
	props->m_Height = 600;
	props->m_Width = 800;
	props->m_Title = "SpaceGame_Client";

	nautilus::hid::HIDProperties* hidProps = new nautilus::hid::HIDProperties();
	hidProps->RegisterKeyboard = true;
	hidProps->RegisterGamepad = true;
	hidProps->RegisterMouse = true;
	hidProps->updateFrequency = nautilus::hid::HIDUpdateFrequency::Fast;
	hidProps->Viewport = glm::vec2(800, 600);

	SpaceGame_Client* client = new SpaceGame_Client();

	if (!client->init(props, hidProps, "ButtonMapping2.data")) return -1;

	// Register main function for playing scene...
	SceneFunctionRegistration* regis = new SceneFunctionRegistration();
	regis->FunctionName = "Main";
	regis->RegisteringFunction = sceneMain;
	client->registerSceneFunction("Empty Scene", regis);



	client->startWithScene("Empty_Scene.scene");


	return 0;
}





void sceneMain(nautilus::graphics::CScene* scene, nautilus::core::Event* eEvent) {

	using namespace std;
	using namespace nautilus;
	using namespace nautilus::core;
	using namespace nautilus::graphics;
	using namespace nautilus::audio;
	using namespace nautilus::network;

	for (auto it : scene->getSceneEntities()) {


		auto& className = it.second->getComponent<ComponentClassName>();
		if (COMPARE_STRINGS(className.m_ClassName, "CSprite") == 0) {

			auto& transformCmp = it.second->getComponent<ComponentTransform>();
			auto& memoryCmp = it.second->getComponent<ComponentMemoryProtocol2D>();
			auto& colorCmp = it.second->getComponent<ComponentGraphics>();
			auto& textureCmp = it.second->getComponent<ComponentTexture2D>();


			glm::mat4 model_transform = glm::mat4(1.0f);
			model_transform = glm::translate(model_transform, glm::vec3(transformCmp.m_Position, 1.0f)) * glm::rotate(model_transform, transformCmp.m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(model_transform, glm::vec3(transformCmp.m_Scale, 1.0f));


			BatchRenderer2D::draw(&memoryCmp, model_transform, &textureCmp, colorCmp.m_Color);
		}
		else if (COMPARE_STRINGS(className.m_ClassName, "CAnimatedSprite") == 0) {

			static_cast<CAnimatedSprite*>(it.second.get())->play(1 / 30.0f);

			auto& transformCmp = it.second->getComponent<ComponentTransform>();
			auto& memoryCmp = it.second->getComponent<ComponentMemoryProtocol2D>();
			auto& colorCmp = it.second->getComponent<ComponentGraphics>();
			auto& textureCmp = it.second->getComponent<ComponentTexture2D>();


			glm::mat4 model_transform = glm::mat4(1.0f);
			model_transform = glm::translate(model_transform, glm::vec3(transformCmp.m_Position, 1.0f)) * glm::rotate(model_transform, transformCmp.m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(model_transform, glm::vec3(transformCmp.m_Scale, 1.0f));


			BatchRenderer2D::draw(&memoryCmp, model_transform, &textureCmp, colorCmp.m_Color);

		}
		else if (COMPARE_STRINGS(className.m_ClassName, "CParticleSystem") == 0) {


			static_cast<CParticleSystem*>(it.second.get())->emit();
			static_cast<CParticleSystem*>(it.second.get())->onRender(1 / 30.0f);
		}


	}


}