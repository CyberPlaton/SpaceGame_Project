#include"Application.h"



namespace nautilus {


	namespace graphics {


		CApplication::CApplication() {

		}


		// Here we initialize the engine
		// and make it ready to use.
		//
		// Further we provide function to close the application and
		// deinitialize the engine and free memory used.
		bool CApplication::init(WindowProperties* wndProps, hid::HIDProperties* props, std::string keyMappingFilename) {

			using namespace nautilus::hid;

			if (!glfwInit()) return false;


			Window* wnd = new Window(wndProps);

			m_Window = Scope<Window>(wnd);


			// Config window and create.
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Setting the opengl version to 3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Usage of modern opengl.


			// Initialize GLEW and OpenGL.
			glewExperimental = GL_TRUE;

			GLenum result = glewInit();
			if (result != GLEW_OK) {

				glfwTerminate();
				return false;
			}


			glfwSetInputMode(m_Window->getWindow(), GLFW_CURSOR, wndProps->m_CursorDisabled);
			glfwSetCursorPos(m_Window->getWindow(), wndProps->m_Width / 2.0, wndProps->m_Height / 2.0);


			// Set clear color.
			glClearColor(wndProps->m_ClearColor.r, wndProps->m_ClearColor.g, wndProps->m_ClearColor.b, wndProps->m_ClearColor.a);

			// Init viewport.
			glViewport(0, 0, wndProps->m_Width, wndProps->m_Height); // Defines where to draw to..



			// Create a scene manager.
			m_SceneManager = Scope<CSceneManager>(CSceneManager::get());
			if (!m_SceneManager) return false;








			// Create input manager.
			std::string buttonFilepath = (COMPARE_STRINGS(keyMappingFilename, "") == 0) ? "Empty" : keyMappingFilename;

			// Here we load the buttons mapping, there is no need to set the mappings
			// in the programm. The only time you want to do that is to initialiy create a mapping file...
			// For this see "HIDManager::_unloadCurrentKeyMap" and create a function to set the mappings and export them.
			bool hid_init = false;
			if (COMPARE_STRINGS(buttonFilepath, "Empty") == 0) {
				hid_init = HIDManager::init(buttonFilepath, props, false);
			}
			else {
				hid_init = HIDManager::init(buttonFilepath, props);
			}

			if(!hid_init) return false;


			m_HIDManager = Scope<HIDManager>(HIDManager::get());
			if (!m_HIDManager) return false;

			m_HIDManager->setViewport(glm::vec2(wndProps->m_Width, wndProps->m_Height));



			// Init sound system
			audio::SoundSystem* audio = new audio::SoundSystem();
			if (!audio->init()) return false;

			m_SoundSystem = Scope<audio::SoundSystem>(audio);

			FPSTimer* timer = new FPSTimer();
			m_FPSTimer = Scope< FPSTimer >(timer);

			return true;
		}




		void CApplication::shutdown() {

			// Shutdown and free input manager.
			nautilus::hid::HIDManager::del();

			// Shutdown and free scene manager and
			// all scenes related stuff.
			m_SceneManager->shutdownScene();
			m_SceneManager.release();


			// Shutdown and release underlying layers like glfw,
			// ImGui etc.
			//
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			glfwTerminate();

		}


		bool CApplication::startWithScene(std::string fileName) {

			if (m_SceneManager->init(fileName)) {

				_applicationMainLoop();
			}
			else {
				return false;
			}
		}


		bool CApplication::transitionToScene(std::string sceneName) {

			return m_SceneManager->transitionToScene(sceneName);
		}


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
		void CApplication::addScene(std::string fileName, std::string scene_from_which, std::string scene_to_which) {

			/*
			Scene transitions:

			scene_from_which --> fileName.
			fileName --> scene_to_which.

			*/


			for (auto it = m_SceneManager->m_SceneTransitionsMap.begin(); it != m_SceneManager->m_SceneTransitionsMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->second, fileName) == 0) {
					m_SceneManager->m_SceneTransitionsMap.emplace(it->first, fileName);
					break;
				}
			}

			m_SceneManager->m_SceneTransitionsMap.emplace(scene_from_which, scene_to_which);
		}





		void CApplication::_onUpdate(float dt) {

			// Update device input...
			glfwPollEvents();

			// Internal update of Eventsystem and user defined update.
			nautilus::core::EventManager::get()->update(dt);
			onUpdate(dt);
		}



		void CApplication::_onInit() {

			m_IsRunning = true;

			// Init renderer
			BatchRenderer2D::init();



			// Init imgui
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;


			glm::vec2 viewport = m_SceneManager->m_ActiveScene->getViewport();

			io.DisplaySize = ImVec2{ viewport.x, viewport.y };



			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::StyleColorsDark();

			if (!ImGui_ImplGlfw_InitForOpenGL(m_Window->getWindow(), false)) return;
			if (!ImGui_ImplOpenGL3_Init("#version 330 core")) return;

			ImGui::CaptureMouseFromApp(false);
			ImGui::CaptureKeyboardFromApp(false);
		}



		void CApplication::_imGuibeginFrame() {

			// Set new frame for Imgui.
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

		}


		void CApplication::_imGuiEndFrame() {

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Display it.
			ImGui::EndFrame();
		}


		void CApplication::_onImGui() {

			onImGuiRendering(); // Draw user defined stuff..
		}



		void CApplication::_render(float dt) {

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);


			BatchRenderer2D::beginScene(m_SceneManager->m_ActiveScene->getViewProjection());


			// Run the "main" function of the active scene.
			m_SceneManager->runScene();


			onRender(dt);
			_onImGui();

			BatchRenderer2D::endScene();


			_imGuiEndFrame();

			glfwSwapBuffers(m_Window->getWindow()); // Double buffered application.

		}


		void CApplication::_applicationMainLoop() {

			// Call users function on creation.
			_onInit();
			onInit();


			while (m_IsRunning) {

				m_FPSTimer->startFrame();
				_imGuibeginFrame();



				_hidManagerUpdate();



				_onUpdate(1 / 60.0f);




				// Internal rendering function.
				// User defined one is called in  "_render".
				_render(1 / 60.0f);



				m_FPSTimer->endFrame();
			}


			onShutdown();
			shutdown();

		}




		void CApplication::_hidManagerUpdate() {

			MSG msg;
			HWND window = glfwGetWin32Window(m_Window->getWindow());
			while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				m_HIDManager->HandleWndMessage(msg);
			}

			// Register user input.
			// Actually, the user input is registered continiously on a separate thread,
			// this function only Updates the underlying "gainput" state.
			//
			// Registered input is dispatched to "EventManager" as an event.
			//
			m_HIDManager->Update();
		}





		void CApplication::onUpdate(float dt){}
		void CApplication::onInit(){}
		void CApplication::onShutdown(){}
		void CApplication::onImGuiRendering(){}
		void CApplication::onRender(float dt){}



		Window::Window(WindowProperties* props) {

			using namespace std;

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);

			cout << color(colors::GREEN);
			cout << "Refresh Rate: " << vidMode->refreshRate << white << endl;


			if (props->m_Fullscreen) {

				m_Window = glfwCreateWindow(vidMode->width, vidMode->height, props->m_Title.c_str(), monitor, NULL);
			
				glfwWindowHint(GLFW_DECORATED, GL_FALSE);

			}
			else {

				m_Window = glfwCreateWindow(props->m_Width, props->m_Height, props->m_Title.c_str(), NULL, NULL);

			}


			if (!m_Window) {
				glfwTerminate();
			}


			glfwMakeContextCurrent(m_Window);

		}


	}



}
