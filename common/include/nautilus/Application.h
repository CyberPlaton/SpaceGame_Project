#pragma once

#include"Base.h"
#include"EventSystem.h"
#include"Renderer.h"
#include"SceneSystem.h"
#include"HIDManager.h"


namespace nautilus {

	namespace graphics {

		// User of Application should implement this functions for 
		// custom callbacks of device inputs...
		// Call back and input from gamepad will be done sperately later..
		extern void GLKeyInputCallback(GLFWwindow* wnd, int key, int scancode, int action, int mode);
		extern void GLMouseMoveCallback(GLFWwindow* wnd, double posX, double posY);
		extern void GLMouseScrollCallback(GLFWwindow* wnd, double deltaX, double deltaY);


		struct WindowProperties {
			int m_Width;
			int m_Height;
			std::string m_Title;
			bool m_Fullscreen;
			bool m_CursorDisabled;
			glm::vec4 m_ClearColor;
		};



		class Window {
		public:
			Window(WindowProperties* props);

			GLFWwindow* getWindow() const { return m_Window; }

		private:
			GLFWwindow* m_Window;
		};






		// What does an application class do?
		// 
		// Well, basically, it represents the actuall game/app instance and
		// defines general functionality that is adherent to it.
		//
		// E.g. initialization and shutdown of the engine framework.
		// Creating a window with specific options, launching the main application loop etc.
		//
		// As an applications are very different, we provide functions to modify
		// the behaiviour of the application without
		// to break the main functionality and without let the user dealing with
		// overly complicated inner workings of the engine...
		//
		//
		// We do not inherit from olc::net::client_interface or olc::net::server_interface
		// so we dont restrict ourselves to a specific type of applications: Multiplayer.
		class CApplication {
		public:

			CApplication();


			// Function creates the app instance
			// and initializes opengl/glfw and creates a window with 
			// specified params.
			//
			// Here we initialize the engine
			// and make it ready to use.
			//
			// Further we provide function to close the application and
			// deinitialize the engine and free memory used.
			bool init(WindowProperties* wndProps, hid::HIDProperties* props, std::string keyMappingFilename = "");
			void shutdown();


			// Setting the targeted Frames per second.
			// E.g. 60.
			//
			void setFramerateGoverning(int fps) { m_FPSTimer->setFpsGoverning(fps); m_FPSTimer->toggleFPSGoverning(); }


			// Functions to order around
			// the scene manager.
			// We want to be able to start the application with a premade scene,
			// to transition to another scene.
			//
			// The start with scene function initializes a scene and
			// prepares to run the scenes "main" function every frame.
			//
			// Returns false on failure.
			bool startWithScene(std::string fileName); // Start the application with given scene file.
			bool transitionToScene(std::string sceneName); // Order a transition to a new scene. Must be already in scene managers scene map.


			// Remember: we only have one loaded and active scene at a time.
			// Thus, here we only add the path of the scene so it can be loaded.
			//
			// Furthermore we define a possible transition.
			// "scene_from_which" defines from which scene we can transit to the new one and
			// "scene_to_which" defines to which scene this one can transit.
			//
			// Alltogether we can define a scene transition TREE with this function.
			//
			// Later we will provide a means to define this tree in a file and load it directly into
			// the scene manager.
			void addScene(std::string fileName, std::string scene_from_which, std::string scene_to_which);



			// Register function for a scene. See "CSceneManager::registerFunctionForScene"
			void registerSceneFunction(std::string sceneName, ISceneFunctionRegistration* regis) {
				
				CSceneManager::get()->registerFunctionForScene(sceneName, regis);
			}



		protected:
			// User specific functions for specifying in detail
			// the behaiviour of the application.
			//
			virtual void onUpdate(float dt);
			virtual void onInit();
			virtual void onShutdown();
			virtual void onImGuiRendering();
			virtual void onRender(float dt);



		protected:

			// As our app is scene based
			// we need a reference to central manager of those.
			//
			// This one is static by default and only one per instance.
			//
			// Thus from the app we can order scene transitions, loadings etc.
			//
			Scope<CSceneManager> m_SceneManager;



			// The Application has an input handler.
			// It manages the input from gamepad, mouse and keyboard.
			// Input registration is done automatically where events on certain inputs are dispatched.
			// The manager is a singleton, so we can make it Scoped.
			//
			Scope<hid::HIDManager> m_HIDManager;




			// The application has a central audio system.
			// Through it we manage playing sound, loading sound etc.
			//
			Scope<audio::SoundSystem> m_SoundSystem;


			Scope< FPSTimer > m_FPSTimer;

		private:

			Scope<Window> m_Window;

			bool m_IsRunning = false;

		private:

			// Internal functions to provide
			// correct application and engine functionality.
			//
			// 
			void _onUpdate(float dt);
			void _onInit();
			void _onImGui();
			void _render(float dt);
			void _hidManagerUpdate();

			void _applicationMainLoop();


			void _imGuibeginFrame();
			void _imGuiEndFrame();
		};



	}



}