#pragma once

#include"GainputInterface.h"


// HID Manager needs to know about Events in order to generate and dispatch them on registered input.
//
#include"EventSystem.h"




// In order to export the mappings to a file or importing from it,
// YAML would be handy.
//
#include"yaml-cpp/yaml.h"


namespace nautilus {


	namespace hid {

		enum class DeviceInputPressureType {
			Not_Pressed = 0,
			Pressed
		};


		enum class HIDUpdateFrequency {
			Slow = 0,
			Normal = 1,
			Fast = 2
		};

		struct HIDProperties {

			// The input update time for the innput thread.
			HIDUpdateFrequency updateFrequency;

			// What kind of device we want to register input from..
			bool RegisterGamepad = false;
			bool RegisterKeyboard = false;
			bool RegisterMouse = false;

			glm::vec2 Viewport = glm::vec2(0.0f);
		};





		class HIDManager {
		public:

			// Button state related function.
			//
			// With them we can check whether button is pressed or held for "x" frames etc.
			// We can further easily check for custom pressing combos.
			//
			//
			bool isButtonPressed(Button button); // Was button pressed last recorded frame?
			bool isButtonHeld(Button button, int frames = 1); // Whether a button was pressed for several frames..
			bool isButtonReleased(Button button); // Whether newly released, where it was previously pressed.

			float getButtonValue(Button button); // Current float value of a trigger or stick.
			float getButtonValueDelta(Button button); // Delta of current frame float value and last frame value of a trigger or stick.







			// Function should be called every update loop,
			// for updating the key state etc.
			//
			void Update() { g_pHIDManager->m_GaInputManager->Update(); _handleInput(); }


			void HandleWndMessage(MSG msg) { g_pHIDManager->m_GaInputManager->HandleMessage(msg); }

			// Function for initializing the manager.
			// Call this function once before accessing the mnger.
			//
			//
			static bool init(std::string fileName, HIDProperties* props, bool loadMap = true) {

				if (!g_pHIDManager) {
					g_pHIDManager = new HIDManager();


					m_Properties = props;
					m_KeymapFilepath = fileName;



					bool init = g_pHIDManager->_init();

					bool loadKeyMap = false;
					if (loadMap) {
						loadKeyMap = g_pHIDManager->_loadKeyMap();
					}


					if (props->Viewport.x <= 1.0f || props->Viewport.y <= 1.0f) return false;
					g_pHIDManager->setViewport(props->Viewport);



					// Initialize the buttonstates.
					for (int i = 0; i < 10; i++) {
						ButtonState state = { DeviceInputPressureType::Not_Pressed };
						g_pHIDManager->m_ButtonStates.push_back(state);
					}


					return (init);
				}

				return false;
			}


			// Set the viewport/display size.
			// This will be the area of input registering.
			//
			//
			void setViewport(glm::vec2 vec);

			 

			// Before using the get function make sure to set properties and
			// specify an input map file.
			//
			static HIDManager* get() {
				if (!g_pHIDManager) return nullptr;
				return g_pHIDManager;
			}


			static void del() {
				g_pHIDManager->_unloadCurrentKeyMap(); // Save input mappings as file.
				delete g_pHIDManager;
			}


			// Setting the filepath for loading and unloading of button mapping file.
			// E.g. ButtonMapping2.data
			//
			static void setButtonFilepath(std::string filename) { HIDManager::m_KeymapFilepath = filename; }
		
			// Function for custom loading of a file.
			// That is,, not on program startup.
			// In order to load correctly, call "setButtonFilepath(...)" with the path to file.
			//
			bool loadButtonMapping() { return g_pHIDManager->_loadKeyMap(); } 

			void setKeyboardButton(Button button, gainput::Key key);
			void setMouseButton(Button button, gainput::MouseButton key);

			// NOTE: it is not possible to set one button to a bool and float value at the same time.
			//
			//
			void setGamepadButton(Button button, gainput::PadButton key);
			void setGamepadButtonFloat(Button button, gainput::PadButton key);

		private:

			typedef std::array< DeviceInputPressureType, Button::Max_Button_Count > ButtonState;
			nautilus::core::tsqueue<ButtonState> m_ButtonStates;

			static HIDManager* g_pHIDManager;



			static HIDProperties* m_Properties;


			// Gainput key map.
			gainput::InputMap* m_InputMap = nullptr;


			// Ginput manager interface.
			gainput::InputManager* m_GaInputManager = nullptr;


			// Ginput device interface.
			gainput::DeviceId m_Keyboard;
			gainput::DeviceId m_Mouse;
			gainput::DeviceId m_Gamepad;


			glm::vec2 m_Viewport;


			inline static std::string m_KeymapFilepath = "";

		private:
			HIDManager() = default;

			// Function initializes the ginput manager,
			// the devices according to the properties.
			//
			bool _init();


			// Store current key-action mappings to yaml file.
			//
			//
			void _unloadCurrentKeyMap();

			// Function for loading the key mappings specified 
			// in the given file.
			// File must be valid and set before initializing the manager.
			//
			bool _loadKeyMap();

			// Store current device input state in our ButtonStates queue.
			//
			void _handleInput(); // Does one frame of input calculation.
		};


	}

}

