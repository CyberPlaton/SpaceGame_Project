#include"HIDManager.h"


namespace nautilus {


	namespace hid {

		HIDManager* HIDManager::g_pHIDManager = nullptr;
		HIDProperties* HIDManager::m_Properties = nullptr;



		void HIDManager::setViewport(glm::vec2 vec) {

			if (!g_pHIDManager) return;
			m_Viewport = vec;
			g_pHIDManager->m_GaInputManager->SetDisplaySize((int)m_Viewport.x, (int)m_Viewport.y);
		}



		bool HIDManager::_init() {

			if (!m_Properties) return false;


			m_GaInputManager = new gainput::InputManager();
			if (!m_GaInputManager) return false;



			m_InputMap = new gainput::InputMap(*m_GaInputManager, "Inputmap");
			if (!m_InputMap) return false;



			if (m_Properties->RegisterGamepad) {

				m_Gamepad = m_GaInputManager->CreateDevice<gainput::InputDevicePad>();
			}

			if (m_Properties->RegisterKeyboard) {

				m_Keyboard = m_GaInputManager->CreateDevice<gainput::InputDeviceKeyboard>();

			}

			if (m_Properties->RegisterMouse) {

				m_Mouse = m_GaInputManager->CreateDevice<gainput::InputDeviceMouse>();

			}

		
			return true;
		}



		void HIDManager::_unloadCurrentKeyMap() {
		
			using namespace YAML;

			std::map<unsigned int, unsigned int> KeyboardMap;
			std::map<unsigned int, unsigned int> PadMap;
			std::map<unsigned int, unsigned int> MouseMap;


			using namespace std;

			const int maxCount = 32;
			gainput::DeviceButtonSpec buttons[maxCount] = { 0 };

			cout << color(colors::DARKRED);
			cout << "Processing Input Map: " << g_pHIDManager->m_InputMap->GetName() << white << endl;


			// Iterate over all defined buttons
			for (int i = Button::Exit; i <= Button::Max_Button_Count; ++i) {

				// Get only mapped buttons
				if (g_pHIDManager->m_InputMap->IsMapped(i) == false) continue;


				const int count = g_pHIDManager->m_InputMap->GetMappings(gainput::UserButtonId(i), buttons, maxCount);


				// Iterate over devices to which the current button is mapped
				for (int j = 0; j < count; ++j) {


					// Get device to which the button is mapped
					gainput::InputDevice* device = g_pHIDManager->m_GaInputManager->GetDevice(buttons[j].deviceId);
					if (device == 0) continue;

					// Get tyep of button
					const gainput::ButtonType buttonType = device->GetButtonType(buttons[j].buttonId);

					if (device->GetType() == gainput::InputDevice::DeviceType::DT_PAD) {

						cout << color(colors::DARKGREEN);
						cout << "Mapping Pad Button:=\"" << i << "\"  to  ButtonID:=\"" << buttons[j].buttonId << "\"." << white << endl;
						PadMap.emplace(i, buttons[j].buttonId);
					}
					else if (device->GetType() == gainput::InputDevice::DeviceType::DT_MOUSE) {

						cout << color(colors::DARKGREEN);
						cout << "Mapping keyboard Button:=\"" << i << "\"  to  ButtonID:=\"" << buttons[j].buttonId << "\"." << white << endl;
						MouseMap.emplace(i, buttons[j].buttonId);

					}
					else if (device->GetType() == gainput::InputDevice::DeviceType::DT_KEYBOARD) {

						cout << color(colors::DARKGREEN);
						cout << "Mapping Mouse Button:=\"" << i << "\"  to  ButtonID:=\"" << buttons[j].buttonId << "\"." << white << endl;
						KeyboardMap.emplace(i, buttons[j].buttonId);

					}



					cout << color(colors::DARKGRAY);
					cout << "Button: " << i << endl;
					cout << "Device ID: " << buttons[j].deviceId << endl;
					cout << "Button ID: " << buttons[j].buttonId << endl;
					cout << "Device Type: " << device->GetTypeName() << endl;
					cout << "Device Index: " << device->GetIndex() << endl;
					cout << "Button Type: " << buttonType << white << endl << endl << endl;
				}

			}


			// Do the actual mapping.
			Emitter out;

			out << BeginMap;
			out << Key << "KeyMapping" << Value << "Version 0.1";


			out << Key << "Mapping" << Value << BeginSeq;



			out << BeginMap;
			out << Key << "Keyboard";
			out << BeginMap;
			out << "Count" << KeyboardMap.size();
			for (auto it: KeyboardMap) {

				out << Key << it.first << it.second;
			}
			out << EndMap;
			out << EndMap;



			out << BeginMap;
			out << Key << "Mouse";
			out << BeginMap;
			out << "Count" << MouseMap.size();
			for (auto it : MouseMap) {

				out << Key << it.first << it.second;
			}
			out << EndMap;
			out << EndMap;



			out << BeginMap;
			out << Key << "Pad";
			out << BeginMap;
			out << "Count" << PadMap.size();
			for (auto it : PadMap) {

				out << Key << it.first << it.second;
			}
			out << EndMap;
			out << EndMap;


			out << EndSeq;
			out << EndMap;


			if (COMPARE_STRINGS(HIDManager::m_KeymapFilepath, "") == 0) {
				cout << color(colors::RED);
				cout << "Forgot to set name of Buttonmapping file! Buttonmapping will be saved as \"ButtonMappingDefault.data\"." << white << endl;
			}

			std::ofstream fout(g_pHIDManager->m_KeymapFilepath);
			fout << out.c_str();
		}


		void HIDManager::setKeyboardButton(Button button, gainput::Key key) {
			g_pHIDManager->m_InputMap->MapBool(button, g_pHIDManager->m_Keyboard, key);
		}


		void HIDManager::setGamepadButton(Button button, gainput::PadButton key) {
			g_pHIDManager->m_InputMap->MapBool(button, g_pHIDManager->m_Gamepad, key);
		}


		void HIDManager::setMouseButton(Button button, gainput::MouseButton key) {
			g_pHIDManager->m_InputMap->MapBool(button, g_pHIDManager->m_Mouse, key);
		}



		bool HIDManager::isButtonPressed(Button button) {

			// Querie the last button state...
			return m_ButtonStates.back()[button] == DeviceInputPressureType::Pressed;
		}


		bool HIDManager::isButtonHeld(Button button, int frames) {

			using namespace std;

			bool down = false;


			// Exclude "frames" with false values...
			//
			// We restrict "frames" value to	{ 1,...,9 }.
			//
			frames = (frames > 9) ? 9 : frames;
			frames = (frames < 1) ? 1 : frames;


			// Reverse iterate through queue.
			for (int i = m_ButtonStates.count() - 1; i >= (m_ButtonStates.count() - frames); i--) {

				down = (m_ButtonStates.getAtIndex(i)[button] == DeviceInputPressureType::Pressed);

			}

			return down;
		}


		bool HIDManager::isButtonReleased(Button button) {


			int index = m_ButtonStates.count() - 2;

			bool down_now = m_ButtonStates.back()[button] == DeviceInputPressureType::Pressed;
			bool down_before = m_ButtonStates.getAtIndex(index)[button] == DeviceInputPressureType::Pressed;


			// Querie last 2 Frames for state of button...
			return (down_before && !down_now);
		}




		void HIDManager::_handleInput() {

			using namespace std;
			using namespace nautilus::core;


			// We are storing up to 10 button states.
			// If we have more than 10 states, delete one.
			if (m_ButtonStates.count() > 9) {
				m_ButtonStates.pop_front(); // Delete first in the queue and thus the "oldest" key state.
			}


			// Detect button states.
			ButtonState newState;

			for (int i = Button::Exit; i < Button::Max_Button_Count; i++) {

				// A button can only be in 2 States.
				// He is being Pressed or not.
				if (m_InputMap->GetBool((Button)i)) {

					newState[i] = DeviceInputPressureType::Pressed;
				}
				else {

					newState[i] = DeviceInputPressureType::Not_Pressed;
				}

			}

			// Store the state of the buttons at this frame.
			//
			m_ButtonStates.push_back(newState);
		}



		bool HIDManager::_loadKeyMap() {

			// First comes the keyboard values,
			// then the mouse values and 
			// in the the pad values.
			//
			using namespace YAML;
			using namespace std;

			std::map<unsigned int, unsigned int> KeyboardMap;
			std::map<unsigned int, unsigned int> PadMap;
			std::map<unsigned int, unsigned int> MouseMap;

			Node data = LoadFile(m_KeymapFilepath);

			if (!data) return false;

			cout << color(colors::GREEN);
			cout << "Loading mapping file \"" << m_KeymapFilepath << "\"." << white << endl;


			auto mapping = data["Mapping"];
			if (mapping) {

				for (auto deviceMap : mapping) {


					auto keyboard = deviceMap["Keyboard"];
					if (keyboard) {

						for (int i = 0; i < keyboard["Count"].as<int>(); i++) {

							// Skip Button values which we dont have defined
							//
							while (!keyboard[i].IsDefined()) {
								i++;
							}

							cout << color(colors::DARKMAGENTA);
							cout << "Mapping Keyboard Button:=\"" << i << "\"  to  ButtonID:=\"" << keyboard[i].as<int>() << "\"." << white << endl;
							KeyboardMap.emplace(i, keyboard[i].as<int>());
						}

					}

					auto mouse = deviceMap["Mouse"];
					if (mouse) {

						for (int i = 0; i < mouse["Count"].as<int>(); i++) {
							 
							// Skip Button values which we dont have defined
							//
							while (! mouse[i].IsDefined()) {
								i++;
							}

							cout << color(colors::DARKMAGENTA);
							cout << "Mapping Mouse Button:=\"" << i << "\"  to  ButtonID:=\"" << mouse[i].as<int>() << "\"." << white << endl;
							MouseMap.emplace(i, mouse[i].as<int>());

						}

					}

					auto pad = deviceMap["Pad"];
					if (pad) {

						for (int i = 0; i < pad["Count"].as<int>(); i++) {

							// Skip Button values which we dont have defined
							//
							while (!pad[i].IsDefined()) {
								i++;
							}

							cout << color(colors::DARKMAGENTA);
							cout << "Mapping Pad Button:=\"" << i << "\"  to  ButtonID:=\"" << pad[i].as<int>() << "\"." << white << endl;
							PadMap.emplace(i, pad[i].as<int>());

						}

					}

				}

			}
			else {
				return false;
			}



			// After the mappings were loaded, shuffle them in the Gainput devices..
			if (g_pHIDManager->m_Properties->RegisterKeyboard) {
				for (auto it : KeyboardMap) {

					g_pHIDManager->setKeyboardButton(Button(it.first), gainput::Key(it.second));
				}
			}

			if (g_pHIDManager->m_Properties->RegisterGamepad) {

				using namespace std;

				for (auto it : PadMap) {

					if (it.first >= 11) {

						g_pHIDManager->setGamepadButtonFloat(Button(it.first), gainput::PadButton(it.second));
					}
					else {

						g_pHIDManager->setGamepadButton(Button(it.first), gainput::PadButton(it.second));
					}

				}
			}


			if (g_pHIDManager->m_Properties->RegisterMouse) {
				for (auto it : MouseMap) {

					g_pHIDManager->setMouseButton(Button(it.first), gainput::MouseButton(it.second));
				}
			}


			return true;
		}



		void HIDManager::setGamepadButtonFloat(Button button, gainput::PadButton key) {
			using namespace std;

			if (!g_pHIDManager->m_InputMap->MapFloat(button, g_pHIDManager->m_Gamepad, key)) {

				cout << color(colors::RED);
				cout << "Could not map Button \""<< button << "\" to gainput::PadButton \""  << key << "\"." << white << endl;
			}
		}




		float HIDManager::getButtonValue(Button button) {

			return g_pHIDManager->m_InputMap->GetFloat(button);
		}



		float HIDManager::getButtonValueDelta(Button button) {

			return g_pHIDManager->m_InputMap->GetFloatDelta(button);
		}




	}

}
