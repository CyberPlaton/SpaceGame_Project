#pragma once

#include"Base.h"
#include"Component.h"
#include"EventSystem.h"
#include"ICamera.h"
#include"Renderer.h"
#include"SoundSystem.h"


#include"common/include/yaml-cpp/yaml.h"



namespace nautilus {


	namespace graphics {

		class CSprite;
		class CParticleSystem;
		class CAnimatedSprite;




		class CSceneManager;
		class CScene;
		class CEntity;
		class CApplication;
		class CSceneSerializer;




		// Class needed for each scene,
		// as here Entt stores the entities, registers/creates them,
		// adds/returns/deletes components etc.
		//
		// Each Scene has an own set of Entities,
		// and thus NEEDS and own instance of this class.
		// And because only one scene can be active at a time,
		// we can easily define this class as static.
		//
		//
		// For implementation see "Scene::EntityRegistry".
		class CEnttRegistry {
			friend class CScene;
			friend class CEntity;
			friend class CSceneSerializer;
		public:

			static CEnttRegistry* get() {
				if (!g_pEnttRegistry) {

					g_pEnttRegistry = new CEnttRegistry();

					// For debugging reasong we do not want entities with the handle 0.
					// Thus on first use we create the first entity and "forget" about it...
					g_pEnttRegistry->m_EntityRegistry.create();
				}

				return g_pEnttRegistry;
			}



			static void del() {
				if (g_pEnttRegistry) delete g_pEnttRegistry;
			}



			static entt::registry& getRegistry() { return CEnttRegistry::get()->m_EntityRegistry; }

		private:

			entt::registry m_EntityRegistry;

			static CEnttRegistry* g_pEnttRegistry;


		private:
			CEnttRegistry() = default;

		};




		// 
		class CScene {
			friend class CEntity;
			friend class CSceneSerializer;
			friend class CSceneManager;

		public:

			CScene(std::string sceneName);

			// Standard scene functionality functions.
			// 
			//

			// Initialize a scene with a camera.
			// A camera is needed in order to display something on the screen.
			//
			bool init(ICamera2D* camera);
			void shutdown();


			// Allow easy creation and destruction of entities for this scene.
			// The entity has by default only the ID component,
			// other components should be added after creation of the entity.
			//
			// E.g.:
			//
			// entt::entity entityHandle = scene->createEntity("Camera");
			//
			// m_pEntity = new CEntity(	entityHandle );
			//
			entt::entity createEntity(std::string tag);

			// The function could be dangerous, 
			// as we free memory in runtime and another object
			// maybe tries to access it and thus accesses a nullptr...
			//
			//
			// For now we leave this function as not implemented.
			// Later, we will let the engine/scenemanager free the memory
			// of the entity on the end of an update loop...
			//
			void destroyEntity(std::string tag);


			// Access the viewport dimensions from the camera.
			//
			glm::vec2 getViewport();

			glm::mat4 getViewProjection() { return m_SceneCamera->getViewProjection(); }



			// Function returns ALL entities in the scene.
			//
			std::map<std::string, Ref<CEntity>> getSceneEntities() { return m_SceneEntities; }


			// Function emits and renders particles of 
			// given particle system. Data for rendering and emitting is used as specified in ParticleData.
			//
			//void renderParticleSystem();


			// Here the implementor/designer/user
			// can specify what the scene should do on a specific event.
			//
			// As each event has a type and a class we can dynamically check for them and
			// decide the resulting action.
			//
			// For example, if an entity reached a specific region of the game map (in 2d space)
			// an event can be fired off, and thus
			// the Scene can request from the scene manager a scene transition,
			// where this scene is unloaded and another is loaded,
			// where the game continues...
			//
			virtual void onEvent(nautilus::core::Event eEvent) {}

			// Here we provide special controll functions for 
			// device input.
			// As the GLFW callbacks do not work for us, we require proper input registration.
			//
			// As the scene is set to active, we will use this implemented functions,
			// but to avoid the issue of rewriting duplicate code, where controlls are same,
			// we want to be able to define a "general" ipnut function which serves
			// as a template for all device input functions.
			//
			// Which scenes should use these template functions should be possible to set dynamically.
			//
			virtual void onKeyInput(nautilus::core::Event eEvent) {}
			virtual void onMouseInput(nautilus::core::Event eEvent) {}
			virtual void onGamepadInput(nautilus::core::Event eEvent) {}


			// Main function for this scene.
			// Here the scene specific functionality is defined.
			//
			// This function will be called every update frame by the engine
			//
			void sceneMain();




			// Functions define what should be done if we load this scene
			// or unload it. Can be used to free memory which is not automatically freed
			// or e.g. to dispatch an event.
			virtual bool onLoad() { return true; }
			virtual void onUnload(){}


		protected:

			// Each scene has its own camera, from which
			// we get the ViewProjection matrix for drawing as well
			// as the viewport for rendering.
			//
			// Without a valid camera a scene cannot function.
			//
			// This interface camera is implemented by the "OrthographicCamera".
			Ref<ICamera2D> m_SceneCamera;


			// Further we have a number of entities 
			// defined for this scene.
			//
			// We store them in a map for possible reference, and easy access by tag.
			// Later the string tag can be replaced with something more storage efficient,
			// e.g. the entt::entity handle. But a string is surely easier for debugging.
			//
			// An iteration is possible too...
			std::map<std::string, Ref<CEntity>> m_SceneEntities;


			// We have further a pointer to our manager,
			// from which we can request actions, like transitioning of scenes etc.
			//
			// This pointer is raw, as I do not like shared pointers at this point...
			//
			CSceneManager* m_SceneManager = nullptr;


			// General things, information.
			std::string m_SceneName;


			// Provides entt functionality. MUST be set in order for the scene to function.
			// For information see "CEnttRegistry".
			Scope< CEnttRegistry > m_EnttRegistry;

		public:

			// Here we can provide functionality for each instance of "CScene".
			// These are intended to be used in the public functions..



			// Get pointer to an entity based on its Handle 
			// or Tag.
			//
			// As long as the values are valid ones,
			// we wil get valid entities returned, as all Entities have a Component 
			// that defines that data.
			Ref<CEntity> getEntity(std::string tag);
			Ref<CEntity> getEntity(entt::entity handle);


		private:

		};







		class CSceneSerializer {
		public:

			static bool serialize(CScene* scene, std::string filepath);

			static  CScene* deserialize(std::string filepath);

		private:

			static void _serializeEntity(YAML::Emitter& out, CEntity e);
		};




		// New struct for custom scene functions.
		struct ISceneFunctionRegistration {
			std::string FunctionName;
			virtual void execute(CScene*, nautilus::core::Event*) = 0;
		};


		struct SceneFunctionRegistration : public ISceneFunctionRegistration {

			// The function must have the type "void func(Event)";
			typedef void(*SceneFunction)(CScene*, nautilus::core::Event*);

			SceneFunction RegisteringFunction;

			void execute(CScene* scene, nautilus::core::Event* eEvent) override {
				RegisteringFunction(scene, eEvent);
			}
		};




		class CSceneManager {
			friend class CApplication;
			friend class CScene;
		public:

			// Functionality for scene manager as game engine addition.

			// We initialize by loading a scene.
			// Loading of a scene is done from file.
			// Thus the scene should have been created before! And serialized,
			// so it can be loaded now.
			//
			// To serialize a scene, just create a function in your main project,
			// define all entities etc. that go in and serialize it...
			bool init(std::string fileName);


			// Initializes a transition from currently active scene to the new 
			// one, as specified by the sceneName.
			//
			// For this, the scene MUST be in the transition map so we can load it.
			// In this function we load the new one, then set it as active and
			// unload the old one. Appopriate functions "onLoad" and "onUnload" are called.
			bool transitionToScene(std::string sceneName);


			// Function call the main function of the active scene,
			// thus running it.
			//
			void runScene();
			bool isSceneRunning() { return m_SceneRunning; }


			// Function calls the "onUnload" function of active scene
			// and releases its unique_ptr, thus deallocating it.
			// If we do not provide a new scene to run, the application shuts down.
			void shutdownScene();




			// Designated functions for loading a specific entitiy into the currently active scene.
			// By loading it, it will be usable in current program.
			// 
			// As we are loading the entities dynamically, while the game is running, 
			// a multithreaded approach could be considered and a means of security for "race conditions" etc.
			//
			//
			bool populateActiveScene(std::string tag, std::string texturename); // CSprite
			bool populateActiveScene(std::string tag, std::string texturename, int textureRows, int textureColumns, float playSpeed); // CAnimatedSprite
			bool populateActiveScene(std::string tag, std::string texturename, ComponentParticleData* pData, ComponentParticlePositionMode* pMode, int particleCount); // CParticleSystem
			
			// Not implemented.
			bool populateActiveScene(audio::SoundSystem* system, std::string tag, std::string soundName, nautilus::audio::PlayOptions op, bool sound2d = true); // Sound
			

			Ref<CEntity> getSceneEntity(std::string entity) {

				return m_ActiveScene->getEntity(entity);
			}

			Ref<CEntity> getSceneEntity(entt::entity entity) {

				return m_ActiveScene->getEntity(entity);
			}



			// Function for registering a scene function.
			void registerFunctionForScene(std::string sceneName, ISceneFunctionRegistration* regis);

			// Function for executing a specific registered function
			// in a scene.
			void runSceneFunction(std::string sceneName, std::string functionName, nautilus::core::Event* eEvent = nullptr);




			static CSceneManager* get() {
				if (!g_pSceneManager) g_pSceneManager = new CSceneManager();

				return g_pSceneManager;
			}

			static void del() {
				if (g_pSceneManager) delete g_pSceneManager;
			}


		private:

			static CSceneManager* g_pSceneManager;

			// The current active scene.
			// As only one scene can be active at a time,
			// we do not need to hold references to others.
			Ref<CScene> m_ActiveScene;
			bool m_SceneRunning = false; // Indicator that shows whther theres a scene that runs its main function.


			// This map defines from which to which scene we can transition.
			// For simplicity we define only one transition path for each scene.
			//
			// This will be tested, so we can see if this suffice.
			//
			// A string represents the path to the scene to be loaded.
			std::map<std::string, std::string> m_SceneTransitionsMap;




		private:

			// For each scene we can provide "override" functions,
			// which the user/designer can call from the "scenemain" function.
			// As each scene has a "main" function, we will search for it in the update loop
			// in order to call it.
			//
			//
			// The scene manager stores those functions for given set of scenes
			// in a map.
			//
			typedef std::vector< ISceneFunctionRegistration* > SceneFunctionsVec;
			std::map<std::string, SceneFunctionsVec> m_RegisteredSceneFunctionsMap;





		private:
			CSceneManager() = default;
		};







		class CEntity {
		public:

			CEntity(CScene* scene, entt::entity handle) : m_EntitiyHandle(handle), m_Scene(scene) {}


			template<typename T, typename... Args>
			T& addComponent(Args&... args) {

				assert(!hasComponent<T>());

				T& cmp = m_Scene->m_EnttRegistry->m_EntityRegistry.emplace<T>(m_EntitiyHandle, std::forward<Args>(args)...);

				return cmp;
			}


			template<typename T>
			bool hasComponent() {
				return m_Scene->m_EnttRegistry->m_EntityRegistry.has<T>(m_EntitiyHandle);
			}


			template<typename T>
			T& getComponent() {
				assert(hasComponent<T>());

				return m_Scene->m_EnttRegistry->m_EntityRegistry.get<T>(m_EntitiyHandle);
			}


			template <typename T>
			void removeComponent() {
				assert(hasComponent<T>());

				m_Scene->m_EnttRegistry->m_EntityRegistry.remove<T>(m_EntitiyHandle);
			}


			operator bool()const { return m_EntitiyHandle != entt::null; }
			operator entt::entity() const { return m_EntitiyHandle; }
			operator uint32_t() const { return (uint32_t)m_EntitiyHandle; }

			bool operator == (const CEntity& rhs)const {
				return m_EntitiyHandle == rhs.m_EntitiyHandle;
			}

			bool operator != (const CEntity& rhs)const {
				return m_EntitiyHandle != rhs.m_EntitiyHandle;
			}


		protected:
			entt::entity m_EntitiyHandle{ entt::null };
			CScene* m_Scene = nullptr;
		};






		class OrthographicCamera : public CEntity, public ICamera2D {
		public:

			// Create camera with transform component.
			// 
			// We need to specify the viewport dimension.
			//
			OrthographicCamera(CScene* scene, std::string tag);



			// Camera has a "ComponentViewport" which holds
			// data about the viewport dimensions,
			// the aspectratio and camera height.
			// The height is stored there as we do not want to polute the 
			// "ComponentTransform" with a camera specific entry.
			bool init(unsigned int viewportWidth, unsigned  int viewportHeight);



			void move(glm::vec2 vec) override; // Move in some direction.
			void move(float x, float y) override; // Move in some direction.
			void elevate(float z) override; // Move up, means increase camera height.
			void sink(float z) override; // Move down, means decrease camera height.



			void teleport(glm::vec2 vec) override; // Move instant to given position.
			void teleport(float x, float y) override; // Move instant to given position.
			void elevateTeleport(float z) override; // .. same.



			glm::vec2 getViewport() const override;



			glm::mat4 getViewProjection() const override { return m_ViewProjectionMatrix; }


			//void move(float x, float y);
			//void moveUp(float z);
			//void moveDown(float z);

			// Function for accessing the view projection matrix to draw the sprite in renderer.
			// The matrix is re-computed every time something changes in the associated values,
			// thus we minimize used time in render-loop,
			// further we give back only one, ready to use matrix to minimize OpenGL uniform calls in order to make renderer faster.
			//glm::mat4 getViewProjection() { return m_ViewProjectionMatrix; }


		private:

			glm::mat4 m_ViewProjectionMatrix;

			// Temporarily solution...
			//
			float m_viewportHeight, m_ViewportWidth;


			glm::mat4 _getLookAt();
			glm::mat4 _getProjection();
			void _recalculateProjection();

		};








		// Note: 
		// A Particle System has a "ComponentParticlePool" for storing the Particles.
 		// For emitting or rendering we access it and use the particles defined there.
		//
		class CParticleSystem : public CEntity {

			// We have to create Objects, to which to apply
			// the data we defined in "ComponentParticleData" and "ComponentParticlePositionMode".
			// And this is this struct here.
			//
			// As the variations for position, color etc are defined in the data structs,
			// we maintain in the particle only the actual, current data which we use
			// for drawing and representing a particle.
			//
			// Thus, changes in position, size, color are computed and set for each particle 
			// dynamically. As such we minimize storage needed for a particle for the cost of computation.
			//
			// Further, this struct IS NOT intended to be imported/exported.
			//

			/*
			struct Particle {

				glm::vec2 ParticlePosition;

				glm::vec2 ParticleVelocity;

				glm::vec4 ParticleColorStart;
				glm::vec4 ParticleColorEnd;



				float ParticleRotation;
				float ParticleRotationSpeed;
				int ParticleRotationDir;

				float ParticleLifetime;
				float ParticleMaxLifetime;

				float ParticleSpeed;

				float ParticleSizeStart;
				float ParticleSizeEnd;


				bool IsActive;
			};
			*/


		public:


			// Because of the entity, we need to reference a scene in the start.
			// We create an entity representing this system and holding all needed data.
			CParticleSystem(CScene* scene, std::string entityTag);


			// As particles can have different textures, 
			// we need to specify it. Later we will request a handle to that texture in the RessourceManager.
			//
			// Further, we do not need to set a shader, as with batch drawing theres a default one.
			//
			// It is neeeded to set the definition for a particle.
			// Even if we deserialize, we only deserialize the data and create a new "CParticleSystem" from it.
			//
			bool init(std::string texturePath, ComponentParticleData* data, ComponentParticlePositionMode* positionMode, int particleCount);


			// Function emit given amount of particles. The amount is set in ParticleData..
			// Emitting means, setting some particles as active, and thus, drawable.
			// 
			// Further we set the data for the activated particle as defined in  "ComponentParticleData"
			// and based on positioning mode as well as randomiztion data...
			void emit();


			// This function is intended to be called in each renderloop.
			//
			// Here we specify the data of active particles based on the lifetime and
			// the elapsed time.
			//
			// Further we call the Renderer::Draw() function here to draw the particles,
			// so user can choose here the drawing "layer".
			void onRender(float dt);


			// Resizing particle pool.
			//
			// This involves destroying all Particles current in the pool and
			// reinitializing to 0. Should not be done at runtime, but on init and shutdown.
			void resizeParticlePool(int n) {

				auto& pool = getComponent< ComponentParticlePool >();

				pool.m_ParticlePool.clear(); pool.m_ParticlePool.resize(n);
			}

		private:
			
			//std::vector<Particle> m_ParticlePool;
			//int m_CurrentParticleIndex = 0;

		private:

		};








	}

}






namespace nautilus {

	namespace graphics {

		// Has an entity.

		class CSprite : public CEntity {
		public:

			// Create a sprite dedicated to given scene.
			CSprite(CScene* scene, std::string entityTag);

			// Initialize components, texture and shader.
			// We do not set the drawing layer here explicitly.
			// As we let for now the user decide dynamically to which "layer" to draw,
			// what can be easily accomplished.
			//
			// As we will use this sprite in the batch shader,
			// we do not set the shader here,
			// the shader is the same for all and set in the renderer.
			bool init(std::string textureName);


			// Common sprite functions.
			void setPosition(glm::vec2 vec);
			void setScale(glm::vec2 vec);
			void setColor(glm::vec4 vec);
			void setRotation(float angle);
			void setBrightness(float n);


		private:


		private:


		};


	}

}









namespace nautilus {

	namespace graphics {

		/*
		The anim. sprite is mostly same as normal sprite,
		but it has some more options.

		Firsly we have the "play" function, which computes the new texture cooordinates
		for the texture provided.

		With this comes, that user must provide more data to work with,
		like rows and columns of the sprite, the wanted play speed.
		The rest will be computed or gathered directly from the texture object.
		*/
		class CAnimatedSprite : public CEntity {
		public:

			// Create a sprite dedicated to given scene.
			CAnimatedSprite(CScene* scene, std::string entityTag);

			// Initialize components, texture and shader.
			// We do not set the drawing layer here explicitly.
			// As we let for now the user decide dynamically to which "layer" to draw,
			// what can be easily accomplished.
			//
			// As we will use this sprite in the batch shader,
			// we do not set the shader here,
			// the shader is the same for all and set in the renderer.
			bool init(std::string textureName, int textureRows, int textureColumns, float playSpeed);


			// Function or advancing the animation cursor and
			// determining whether to set a "new frame" as current sprite.
			void play(float dt);


			// Common sprite functions.
			void setPosition(glm::vec2 vec);
			void setScale(glm::vec2 vec);
			void setColor(glm::vec4 vec);
			void setRotation(float angle);
			void setBrightness(float n);

		private:


		private:


		};


	}

}