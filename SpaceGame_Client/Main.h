#pragma once

#include"EngineInterface.h"

#ifdef _DEBUG
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "yaml-cppd.lib")
#pragma comment(lib, "gainput-d.lib")
#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fsbank_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")
#pragma comment(lib, "lua54.lib")
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "yaml-cpp.lib")
#pragma comment(lib, "gainput.lib")
#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fsbank_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")
#pragma comment(lib, "lua54.lib")
#pragma comment(lib, "steam_api64.lib")
#endif



// The client can be state machine based and depending on the state of client
// can then transit to scenes and render theyre entities.
//
// The state is then determined by whether the client is connected to a server,
// whether the client is in the "main menu" or "server lobby" or "in game".
// Thus the "main menu" and others are scenes in the context of the game engine.
// 
// 
// 
//
class SpaceGame_Client : public nautilus::graphics::CApplication, public olc::net::client_interface<nautilus::network::NetMsg> {
public:

	SpaceGame_Client() {

		m_Timer = new nautilus::HRTimer();
	}


	void onUpdate(float dt) override;


	void onInit() override;


	void onShutdown() override {

	}


	void onImGuiRendering() override {

		ImGui::Begin("Statistics");
		ImGui::Text("Average %.3f ms/frame (%.1f FPS) ", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("%.3f Frametime --- %.3f ms/frame --- (%.3f FPS) ", m_FPSTimer->getAverageFrametime(), m_FPSTimer->getAverageMillisecondsPerFrame(), m_FPSTimer->getAverageFPS());
		ImGui::Text("Average %.3f ping (%.1f last ping) ", _getAveragePing(), m_LastPing);
		ImGui::End();
	}



	void onRender(float dt) override {

	}



private:

	// Network statistics related.
	// Measuring Ping.
	//std::vector<float> m_PingVec;
	nautilus::core::tsqueue<float> m_PingQueue;
	nautilus::HRTimer* m_Timer = nullptr;
	float m_LastPing = 0.0f;



	// Our player instance.
	uint32_t m_PlayerID = 0;
	nautilus::network::PlayerDescription m_PlayerDesc;


	// Other player entities.
	std::unordered_map<uint32_t, nautilus::network::PlayerDescription> m_PlayerLobby;


	bool m_WaitingForConnection = true;


	// Engine related.
	//InputDelegate* m_ButtonInputDelegate = nullptr;
	//InputDelegateRelease* m_ButtonInputDelegateRelease = nullptr;

private:

	float _getAveragePing() {

		float p = 0.0f;
		int size = m_PingQueue.count();

		for (int i = 0; i < size; i++) {


			p += m_PingQueue.pop_front();
		}

		return p / size;
	}
};









/*
class InputDelegate : public nautilus::core::BaseEventDelegate {
public:
	InputDelegate(SpaceGame_Client* player) : m_Client(player) {

	}


	void execute(nautilus::core::Event* userEvent) override;


	SpaceGame_Client* m_Client = nullptr;

};



class InputDelegateRelease : public nautilus::core::BaseEventDelegate {
public:
	InputDelegateRelease(SpaceGame_Client* player) : m_Client(player) {

	}


	void execute(nautilus::core::Event* userEvent) override;


	SpaceGame_Client* m_Client = nullptr;

};
*/

void sceneMain(nautilus::graphics::CScene* scene, nautilus::core::Event* eEvent);
