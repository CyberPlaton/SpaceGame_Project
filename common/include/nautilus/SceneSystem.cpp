#include"SceneSystem.h"

namespace YAML {

	// Expand functionality of YAML for custom conversion
	// of glm vectors...

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};



	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};







	// Helpers for YAML emitt.
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	// Helpers for YAML emitt.
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	// Helpers for YAML emitt.
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}



}







namespace nautilus{

	namespace graphics {


		CEnttRegistry* CEnttRegistry::g_pEnttRegistry = nullptr;

		CSceneManager* CSceneManager::g_pSceneManager = nullptr;




		// Initialize a scene with a camera.
		// A camera is needed in order to display something on the screen.
		//
		bool CScene::init(ICamera2D* camera) {

			return true;
		}



		CScene::CScene(std::string sceneName) {

			m_SceneName = sceneName;


			m_EnttRegistry = Scope<CEnttRegistry>(CEnttRegistry::get());
		}


		void CScene::shutdown() {

			for (auto it = m_SceneEntities.begin(); it != m_SceneEntities.end(); it++) {

				it->second.reset();
			}

			m_SceneEntities.clear();
		}


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
		entt::entity CScene::createEntity(std::string tag) {


			// Create entity.
			entt::entity handle = m_EnttRegistry->getRegistry().create(); // Make entt handle..

			Ref<CEntity> entity = Ref<CEntity>(new CEntity(this, handle)); // .. make entity from it..
			

			auto& id = entity->addComponent<ComponentID>();
			id.m_ID = (uint32_t)handle;
			id.m_Tag = tag;


			m_SceneEntities.emplace(tag, entity); // .. and store it in our map.


			return handle;
		}



		// Not implemented.
		void CScene::destroyEntity(std::string tag) {


		}


		void CScene::sceneMain() {

			// To run this scenes main function we need to construct
			// arguments correctly.
			//
			// E.g. scenename = "Main_Level.scene".
			//		scenfunc = "Main".
			//
			// Further the main function does not accept any events. It is run every update step.
			//
			std::string sceneName = m_SceneName;
			std::string sceneFunctionName = "Main";

			m_SceneManager->runSceneFunction(sceneName, sceneFunctionName);
		}



		glm::vec2 CScene::getViewport() {

			return m_SceneCamera->getViewport();
		}



		// Get pointer to an entity based on its Handle 
		// or Tag.
		//
		// As long as the values are valid ones,
		// we wil get valid entities returned, as all Entities have a Component 
		// that defines that data.
		Ref<CEntity> CScene::getEntity(std::string tag) {


			for (auto it = m_SceneEntities.begin(); it != m_SceneEntities.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, tag) == 0) return it->second;
			}

			return nullptr;
		}


		Ref<CEntity> CScene::getEntity(entt::entity handle) {


			for (auto it = m_SceneEntities.begin(); it != m_SceneEntities.end(); it++) {

				if ((uint32_t)*it->second == (uint32_t)handle) return it->second;
			}

			return nullptr;
		}


























		// We initialize by loading a scene.
		// Loading of a scene is done from file.
		// Thus the scene should have been created before! And serialized,
		// so it can be loaded now.
		//
		// To serialize a scene, just create a function in your main project,
		// define all entities etc. that go in and serialize it...
		bool CSceneManager::init(std::string fileName) {

			CScene* scene = nullptr;

			scene = CSceneSerializer::deserialize(fileName);
			if (!scene) return false;



			scene->m_SceneManager = this;
			m_ActiveScene = Ref<CScene>(scene);

			if (!m_ActiveScene->onLoad()) {

				m_ActiveScene->onUnload();
				m_ActiveScene.reset();
				return false;
			}

			return true;
		}


		// Initializes a transition from currently active scene to the new 
		// one, as specified by the sceneName.
		//
		// For this, the scene MUST be in the transition map so we can load it.
		// In this function we load the new one, then set it as active and
		// unload the old one. Appopriate functions "onLoad" and "onUnload" are called.
		bool CSceneManager::transitionToScene(std::string sceneName) {

			CScene* scene = CSceneSerializer::deserialize(sceneName);
			if (!scene) return false;

			m_SceneRunning = false;
			m_ActiveScene->onUnload();


			m_ActiveScene = Scope<CScene>(scene);
			m_ActiveScene->onLoad();
			m_SceneRunning = true;

			return true;
		}



		void CSceneManager::runScene() {

			m_SceneRunning = true;
			m_ActiveScene->sceneMain();
		}


		void CSceneManager::shutdownScene() {

			m_ActiveScene->onUnload();
			m_ActiveScene.reset();
			m_SceneRunning = false;
		}




		void CSceneManager::registerFunctionForScene(std::string sceneName, ISceneFunctionRegistration* regis) {

			for (auto it = m_RegisteredSceneFunctionsMap.begin(); it != m_RegisteredSceneFunctionsMap.end(); it++) {

				// Scene exists in registration map
				if (COMPARE_STRINGS_2(it->first, sceneName) == 0) {

					it->second.push_back(regis);
				}

			}


			// Scene does not exist in registration map
			std::vector< ISceneFunctionRegistration* > vec; vec.push_back(regis);
			m_RegisteredSceneFunctionsMap.emplace(sceneName, vec);
		}


		void CSceneManager::runSceneFunction(std::string sceneName, std::string functionName, nautilus::core::Event* eEvent) {

			for (auto it = m_RegisteredSceneFunctionsMap.begin(); it != m_RegisteredSceneFunctionsMap.end(); it++) {

				// Scene exists in registration map
				if (COMPARE_STRINGS_2(it->first, sceneName) == 0) {

					for (auto itr : it->second) {

						if (COMPARE_STRINGS_2(itr->FunctionName, functionName) == 0) {
							itr->execute(m_ActiveScene.get(), eEvent);
							return;
						}
					}
				}

			}
		}




		bool CSceneManager::populateActiveScene(std::string tag, std::string texturename) {

			CSprite* sprite = nullptr; sprite = new CSprite(m_ActiveScene.get(), tag);


			if (!sprite) return false;

			return sprite->init(texturename);
		}



		bool CSceneManager::populateActiveScene(std::string tag, std::string texturename, int textureRows, int textureColumns, float playSpeed) {

			CAnimatedSprite* sprite = nullptr; sprite = new CAnimatedSprite(m_ActiveScene.get(), tag);

			if (!sprite) return false;

			return sprite->init(texturename, textureRows, textureColumns, playSpeed);
		}



		bool CSceneManager::populateActiveScene(std::string tag, std::string texturename, ComponentParticleData* pData, ComponentParticlePositionMode* pMode, int particleCount) {

			CParticleSystem* system = nullptr; system = new CParticleSystem(m_ActiveScene.get(), tag);

			if (!system) return false;

			return system->init(texturename, pData, pMode, particleCount);
		}



		bool CSceneManager::populateActiveScene(audio::SoundSystem* system, std::string tag, std::string soundName, nautilus::audio::PlayOptions op, bool sound2d) {

			using namespace nautilus::audio;

			// Use sound system of appliction given through pointer.
			return false;
			
		}







		bool CSceneSerializer::serialize(CScene* scene, std::string filepath) {


			using namespace YAML;
			YAML::Emitter out;

			out << BeginMap;
			out << Key << "Scene" << Value << scene->m_SceneName;

			out << Key << "Entities" << Value << BeginSeq;

			scene->m_EnttRegistry->getRegistry().each([&](auto entity) {

				CEntity e = { scene, entity};
				if (!e) return;

				// We added a null_entity to the registry.
				// It holds no components and no information, thus we do not want it to be serialized!
				//
				if (0 == e.operator entt::id_type()) return;

				_serializeEntity(out, e);

				});

			out << EndSeq;
			out << EndMap;

			std::ofstream fout(filepath);
			fout << out.c_str();

			return true;
			
		}


		void CSceneSerializer::_serializeEntity(YAML::Emitter& out, CEntity e) {

			using namespace YAML;
			out << BeginMap; // An entity map



			if (e.hasComponent<ComponentViewport>()) {

				out << Key << "ComponentViewport";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentViewport>();


				out << Key << "AspectRatio" << cmp.m_AspectRatio;
				out << Key << "ViewportWidth" << cmp.m_ViewportWidth;
				out << Key << "ViewportHeight" << cmp.m_ViewportHeight;
				out << Key << "CameraHeight" << cmp.m_CameraHeight;


				out << EndMap;
			}


			if (e.hasComponent<ComponentID>()) {

				out << Key << "ComponentID";
				out << BeginMap;

				auto& id = e.getComponent<ComponentID>();

				out << Key << "Tag" << Value << id.m_Tag;
				out << Key << "ID" << Value << id.m_ID;

				out << EndMap;
			}


			if (e.hasComponent<ComponentTransform>()) {

				out << Key << "ComponentTransform";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentTransform>();

				out << Key << "Position" << Value << cmp.m_Position;
				out << Key << "Scale" << Value << cmp.m_Scale;
				out << Key << "Rotation" << Value << cmp.m_Rotation;

				out << EndMap;
			}


			if (e.hasComponent<ComponentGraphics>()) {

				out << Key << "ComponentGraphics";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentGraphics>();

				out << Key << "Color" << Value << cmp.m_Color;

				out << EndMap;
			}



			if (e.hasComponent<ComponentTexture2D>()) {

				out << Key << "ComponentTexture2D";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentTexture2D>();

				out << Key << "Filepath" << Value << cmp.m_FilePath;

				out << EndMap;
			}


			if (e.hasComponent<ComponentShader>()) {

				out << Key << "ComponentShader";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentShader>();

				// For the shader we need to delete the .vert and .frag keywords.
				std::string name = cmp.m_FilePath.substr(0, cmp.m_FilePath.size() - 5); // Remove the word ".vert".

				out << Key << "Filepath" << Value << name;


				out << EndMap;
			}


			if (e.hasComponent<ComponentScript>()) {

				out << Key << "ComponentScript";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentScript>();

				// For the script we only save the path to the file.
				out << Key << "Filepath" << Value << cmp.m_Script->m_Filepath;

				out << EndMap;
			}


			if (e.hasComponent<ComponentMemoryProtocol2D>()) {

				auto& cmp = e.getComponent<ComponentMemoryProtocol2D>();


				out << Key << "ComponentMemoryProtocol2D";
				out << BeginMap;

				// We want to emit 4 vec2 objects,
				// we do this as a map.

				out << Key << "Vector";
				out << BeginMap;
				out << Key << "0" << cmp.m_TextureCoords[0];
				out << Key << "1" << cmp.m_TextureCoords[1];
				out << Key << "2" << cmp.m_TextureCoords[2];
				out << Key << "3" << cmp.m_TextureCoords[3];
				out << EndMap;

				out << EndMap;
			}


			if (e.hasComponent<ComponentAnimationData>()) {

				out << Key << "ComponentAnimationData";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentAnimationData>();

				out << Key << "TextureWidth" << cmp.m_TextureWidth;
				out << Key << "TextureHeight" << cmp.m_TextureHeight;
				out << Key << "FrameWidth" << cmp.m_FrameWidth;
				out << Key << "FrameHeight" << cmp.m_FrameHeight;
				out << Key << "PlaySpeed" << cmp.m_PlaySpeed;
				out << Key << "AnimationCursor" << cmp.m_AnimationCursor;
				out << Key << "CurrentFrameX" << cmp.m_CurrentFrameX;
				out << Key << "CurrentFrameY" << cmp.m_CurrentFrameY;
				out << Key << "AnimationRows" << cmp.m_AnimationRows;
				out << Key << "AnimationColumns" << cmp.m_AnimationColumns;

				out << EndMap;
			}



			if (e.hasComponent< ComponentParticlePositionMode >()) {

				out << Key << "ComponentParticlePositionMode";
				out << BeginMap;
			
				auto& cmp = e.getComponent<ComponentParticlePositionMode>();

			
				if (cmp.m_PositionMode == ComponentParticlePositionMode::Mode::Fixed_To_Position) {

					out << Key << "Mode" << "Fixed_To_Position";
					out << Key << "Position" << cmp.m_Mode_Fixed_To_Position->ModePosition;

				}
				else if (cmp.m_PositionMode == ComponentParticlePositionMode::Mode::Fixed_To_Space) {

					out << Key << "Mode" << "Fixed_To_Space";
					out << Key << "Point" << cmp.m_Mode_Fixed_To_Space->Point;
					out << Key << "RectWidth" << cmp.m_Mode_Fixed_To_Space->RectWidth;
					out << Key << "RectHeight" << cmp.m_Mode_Fixed_To_Space->RectHeight;

				}
				else if (cmp.m_PositionMode == ComponentParticlePositionMode::Mode::Following_Entity) {

					out << Key << "Mode" << "Following_Entity";
					out << Key << "EntityHandle" << cmp.m_Mode_Following_Entity->EntityHandle;
				}
				

				out << EndMap;
			}




			if (e.hasComponent< ComponentParticleData >()) {

				out << Key << "ComponentParticleData";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentParticleData>();



				out << Key << "ColorEnd" << cmp.ColorEnd;
				out << Key << "ColorEndVar" << cmp.ColorEndVar;
				out << Key << "ColorStart" << cmp.ColorStart;
				out << Key << "ColorStartVar" << cmp.ColorStartVar;
				out << Key << "Emission" << cmp.EmissionRate;
				out << Key << "Lifetime" << cmp.MaxLifetime;
				out << Key << "LifetimeVar" << cmp.MaxLifeTimeVar;
				out << Key << "Position" << cmp.Position;
				out << Key << "PositionVar" << cmp.PositionVar;
				out << Key << "Rotation" << cmp.Rotation;
				out << Key << "RotationDir" << cmp.RotationDir;
				out << Key << "RotationSpeed" << cmp.RotationSpeed;
				out << Key << "RotationSpeedVar" << cmp.RotationSpeedVar;
				out << Key << "RotationVar" << cmp.RotationVar;
				out << Key << "SizeEnd" << cmp.SizeEnd;
				out << Key << "SizeEndVar" << cmp.SizeEndVar;
				out << Key << "SizeStart" << cmp.SizeStart;
				out << Key << "SizeStartVar" << cmp.SizeStartVar;
				out << Key << "Speed" << cmp.Speed;
				out << Key << "SpeedVar" << cmp.SpeedVar;
				out << Key << "Velocity" << cmp.Velocity;
				out << Key << "VelocityVar" << cmp.VelocityVar;
				out << Key << "ParticleCount" << cmp.ParticleCount;



				out << EndMap;
			}


			if (e.hasComponent< ComponentClassName >()) {

				out << Key << "ComponentClassName";
				out << BeginMap;

				auto& cmp = e.getComponent<ComponentClassName>();
				out << Key << "ClassName" << cmp.m_ClassName;

				out << EndMap;
			}



			out << EndMap;

		}




		CScene* CSceneSerializer::deserialize(std::string filepath) {

			using namespace YAML;
			Node data = LoadFile(filepath);
			CScene* scene = nullptr;

			scene = new CScene(data["Scene"].as<std::string>());

			if (!scene) return nullptr;


			// Deserialize...
			auto entities = data["Entities"];
			if (entities) {


				for (auto entity : entities) {

					CEntity* deserializedEntity = nullptr;

					// Each entity has a class name defined in theyre serialization.
					// We need this in order to create an object as 
					// we deserialize it.
					//
					// For this we have a map defined, which holds possible string to class mappings.
					// 
					// Note:
					//	If we deserialize and create a new component for an entity, we have to initialize it correctly and in place,
					//  as in the init-function of the CEntity we will skip this step!
					//
					auto entityClass = entity["ComponentClassName"];
					auto id = entity["ComponentID"];
					if (entityClass) {

						if (COMPARE_STRINGS(entityClass["ClassName"].as<std::string>(), "CSprite") == 0) {

							// Create an emtpy entity of type 
							// CSprite. Here the entities do not have any component BUT the "ComponentID", which is basic for ALL.
							//
							deserializedEntity = new CSprite(scene, id["Tag"].as<std::string>());

							auto& cmp = deserializedEntity->addComponent<ComponentClassName>();
							cmp.m_ClassName = "CSprite";

						}
						else if (COMPARE_STRINGS(entityClass["ClassName"].as<std::string>(), "CAnimatedSprite") == 0) {

							deserializedEntity = new CAnimatedSprite(scene, id["Tag"].as<std::string>());

							auto& cmp = deserializedEntity->addComponent<ComponentClassName>();
							cmp.m_ClassName = "CAnimatedSprite";
						}
						else if (COMPARE_STRINGS(entityClass["ClassName"].as<std::string>(), "CParticleSystem") == 0) {

							deserializedEntity = new CParticleSystem(scene, id["Tag"].as<std::string>());
							



							// Create ParticleSystem in place, as this is a "Special Case"... Really fucked up to do this...
							auto particleData = entity["ComponentParticleData"];
							if (particleData) {

								if (!deserializedEntity->hasComponent< ComponentParticleData >() && !deserializedEntity->hasComponent< ComponentParticlePositionMode >()) {


									auto& cmp = deserializedEntity->addComponent< ComponentParticleData >();

									// Create particle data.
									cmp.ColorEnd = particleData["ColorEnd"].as<glm::vec4>();
									cmp.ColorEndVar = particleData["ColorEndVar"].as<glm::vec4>();
									cmp.ColorStart = particleData["ColorStart"].as<glm::vec4>();
									cmp.ColorStartVar = particleData["ColorStartVar"].as<glm::vec4>();

									cmp.Position = particleData["Position"].as<glm::vec2>();
									cmp.PositionVar = particleData["PositionVar"].as<glm::vec2>();
									cmp.Velocity = particleData["Velocity"].as<glm::vec2>();
									cmp.VelocityVar = particleData["VelocityVar"].as<glm::vec2>();

									cmp.MaxLifetime = particleData["Lifetime"].as<float>();
									cmp.SizeStart = particleData["SizeStart"].as<float>();
									cmp.SizeEnd = particleData["SizeEnd"].as<float>();
									cmp.Rotation = particleData["Rotation"].as<float>();
									cmp.RotationDir = particleData["RotationDir"].as<float>();
									cmp.RotationSpeed = particleData["RotationSpeed"].as<float>();
									cmp.Speed = particleData["Speed"].as<float>();
									cmp.MaxLifeTimeVar = particleData["LifetimeVar"].as<float>();
									cmp.SizeStartVar = particleData["SizeStartVar"].as<float>();
									cmp.SizeEndVar = particleData["SizeEndVar"].as<float>();
									cmp.RotationVar = particleData["RotationVar"].as<float>();
									cmp.RotationSpeedVar = particleData["RotationSpeedVar"].as<float>();
									cmp.SpeedVar = particleData["SpeedVar"].as<float>();
									cmp.EmissionRate = particleData["Emission"].as<float>();


									cmp.ParticleCount = particleData["ParticleCount"].as<unsigned int>();



									// Create particle position mode
									auto particlePositionMode = entity["ComponentParticlePositionMode"];
									if (particlePositionMode) {

										auto& cmp = deserializedEntity->addComponent< ComponentParticlePositionMode >();

										std::string mode = particlePositionMode["Mode"].as<std::string>();

										if (COMPARE_STRINGS(mode, "Fixed_To_Position") == 0) {
											cmp.m_PositionMode = ComponentParticlePositionMode::Mode::Fixed_To_Position;

											cmp.m_Mode_Fixed_To_Position = new ComponentParticlePositionMode::Mode_Fixed_To_Position();
											cmp.m_Mode_Fixed_To_Position->ModePosition = particlePositionMode["Position"].as<glm::vec2>();
										}
										else if (COMPARE_STRINGS(mode, "Fixed_To_Space") == 0) {

											cmp.m_PositionMode = ComponentParticlePositionMode::Mode::Fixed_To_Space;

											cmp.m_Mode_Fixed_To_Space = new ComponentParticlePositionMode::Mode_Fixed_To_Space();
											cmp.m_Mode_Fixed_To_Space->Point = particlePositionMode["Point"].as<glm::vec2>();
											cmp.m_Mode_Fixed_To_Space->RectHeight = particlePositionMode["RectHeight"].as<float>();
											cmp.m_Mode_Fixed_To_Space->RectWidth = particlePositionMode["RectWidth"].as<float>();


										}
										else if (COMPARE_STRINGS(mode, "Following_Entity") == 0) {

											cmp.m_PositionMode = ComponentParticlePositionMode::Mode::Following_Entity;

											cmp.m_Mode_Following_Entity = new ComponentParticlePositionMode::Mode_Following_Entity();

											cmp.m_Mode_Following_Entity->EntityHandle = particlePositionMode["EntityHandle"].as<std::string>();
										}
									}
								}

							}

							auto& pData = deserializedEntity->getComponent< ComponentParticleData >();
							auto& pMode = deserializedEntity->getComponent< ComponentParticlePositionMode >();

							auto texture = entity["ComponentTexture2D"];
							std::string texturepath = texture["Filepath"].as<std::string>();


							static_cast<CParticleSystem*>(deserializedEntity)->init(texturepath, &pData, &pMode, particleData["ParticleCount"].as<unsigned int>());



							//auto& cmp = deserializedEntity->addComponent<ComponentClassName>();
							//cmp.m_ClassName = "CParticleSystem";
						}
						else if (COMPARE_STRINGS(entityClass["ClassName"].as<std::string>(), "OrthographicCamera") == 0) {

							deserializedEntity = new OrthographicCamera(scene, id["Tag"].as<std::string>());

							auto& cmp = deserializedEntity->addComponent<ComponentClassName>();
							cmp.m_ClassName = "OrthographicCamera";
						}

					}
					else {

						using namespace std;
						cout << color(colors::RED);
						cout << "Entity has no \"ComponentClassName\":\n Entity: " << id["Tag"].as<std::string>() << endl;
						cout << "Scene File: " << filepath << white << endl;
					}



					// This should be checked first, as we know that all have this component,
					// and we need the data to correctly construct the entity.
					//
					//
					// Here we create the entity and store it in the scenes entity map.
					//
					/*
					auto id = entity["ComponentID"];
					if (id) {

						entt::entity handle = scene->createEntity(id["Tag"].as<std::string>());
						deserializedEntity = Ref<CEntity>(new CEntity(scene, handle));

						scene->m_SceneEntities.emplace(id["Tag"].as<std::string>(), Ref<CEntity>(deserializedEntity));
					}
					else {
						return nullptr;
					}
					*/



					// Now all the others...
					//

					auto viewport = entity["ComponentViewport"];
					if (viewport) {

						if (!deserializedEntity->hasComponent< ComponentViewport >()) {


							auto& cmp = deserializedEntity->addComponent< ComponentViewport >();

							cmp.m_AspectRatio = viewport["AspectRatio"].as<float>();
							cmp.m_CameraHeight = viewport["CameraHeight"].as<float>();
							cmp.m_ViewportHeight = viewport["ViewportHeight"].as<float>();
							cmp.m_ViewportWidth = viewport["ViewportWidth"].as<float>();


							// As we have here the definitions for the camera, we will use it in place
							// to initialize the previously created camera.
							//

							static_cast<OrthographicCamera*>(deserializedEntity)->init(cmp.m_ViewportWidth, cmp.m_ViewportHeight);
							static_cast<OrthographicCamera*>(deserializedEntity)->elevate(cmp.m_CameraHeight);

							scene->m_SceneCamera = Ref<OrthographicCamera>(static_cast<OrthographicCamera*>(deserializedEntity));
						}

					}



					auto transform = entity["ComponentTransform"];
					if (transform) {

						// Protection against "reinitializing" components.
						if (!deserializedEntity->hasComponent< ComponentTransform >()) {


							auto& cmp = deserializedEntity->addComponent< ComponentTransform >();

							cmp.m_Position = transform["Position"].as<glm::vec2>();
							cmp.m_Scale = transform["Scale"].as<glm::vec2>();
							cmp.m_Rotation = transform["Rotation"].as<float>();
						}
					}



					auto graphics = entity["ComponentGraphics"];
					if (graphics) {

						if (!deserializedEntity->hasComponent< ComponentGraphics >()) {

							auto& cmp = deserializedEntity->addComponent< ComponentGraphics >();

							cmp.m_Color = graphics["Color"].as<glm::vec4>();
						}

					}




					auto texture = entity["ComponentTexture2D"];
					if (texture) {

						if (!deserializedEntity->hasComponent< ComponentTexture2D >()) {

							auto& cmp = deserializedEntity->addComponent< ComponentTexture2D >();

							// The texture needs to be loaded,
							// this is what we do here.
							//
							//
							cmp.m_FilePath = texture["Filepath"].as<std::string>();
							cmp.init(cmp.m_FilePath);
						}

					}




					auto shader = entity["ComponentShader"];
					if (shader) {

						if (!deserializedEntity->hasComponent< ComponentShader >()) {


							auto& cmp = deserializedEntity->addComponent< ComponentShader >();

							// Just like the texture, we have to initialize the shader,
							// means load and compile it,
							// this is what we do here.
							//
							cmp.m_FilePath = shader["Filepath"].as<std::string>();
							cmp.init(cmp.m_FilePath);
						}

					}



					auto script = entity["ComponentScript"];
					if (script) {

						if (!deserializedEntity->hasComponent< ComponentScript >()) {

							auto& cmp = deserializedEntity->addComponent< ComponentScript >();

							// The script needs to be initialized by loading it.
							// This is what we do here...
							//
							//
							cmp.init(script["Filepath"].as<std::string>());
						}

					}




					auto memory = entity["ComponentMemoryProtocol2D"];
					if (memory) {

						if (!deserializedEntity->hasComponent< ComponentMemoryProtocol2D >()) {


							auto& cmp = deserializedEntity->addComponent< ComponentMemoryProtocol2D >();


							glm::vec2 first, second, third, fourth;

							// Extract vectors from the file.
							//

							first = memory["Vector"]["0"].as<glm::vec2>();
							second = memory["Vector"]["1"].as<glm::vec2>();
							third = memory["Vector"]["2"].as<glm::vec2>();
							fourth = memory["Vector"]["3"].as<glm::vec2>();

							// And assign them to texture coords..
							// 
							cmp.m_TextureCoords[0] = first;
							cmp.m_TextureCoords[1] = second;
							cmp.m_TextureCoords[2] = third;
							cmp.m_TextureCoords[3] = fourth;
						}
						
					}



					auto renderable = entity["ComponentRenderableEntity"];
					if (renderable) {

						if (!deserializedEntity->hasComponent< ComponentRenderableEntity >()) {


							auto& cmp = deserializedEntity->addComponent< ComponentRenderableEntity >();

							cmp.m_IsAnimated = renderable["Animated"].as<bool>();
							cmp.m_IsRenderable = renderable["Renderable"].as<bool>();
						}

					}




					auto animation = entity["ComponentAnimationData"];
					if (animation) {

						if (!deserializedEntity->hasComponent< ComponentAnimationData >()) {


							auto& cmp = deserializedEntity->addComponent< ComponentAnimationData >();


							cmp.m_AnimationColumns = animation["AnimationColumns"].as<int>();
							cmp.m_AnimationRows = animation["AnimationRows"].as<int>();
							cmp.m_AnimationCursor = animation["AnimationCursor"].as<float>();
							cmp.m_CurrentFrameX = animation["CurrentFrameX"].as<int>();
							cmp.m_CurrentFrameY = animation["CurrentFrameY"].as<int>();
							cmp.m_FrameHeight = animation["FrameHeight"].as<float>();
							cmp.m_FrameWidth = animation["FrameWidth"].as<float>();
							cmp.m_PlaySpeed = animation["PlaySpeed"].as<float>();
							cmp.m_TextureHeight = animation["TextureHeight"].as<float>();
							cmp.m_TextureWidth = animation["TextureWidth"].as<float>();

						}

					}



					// This is some special stuff for the particle system.
					// We need two components below to initialize the "CParticleSystem", so
					// for now we will check for both and create one.
					//
					// Later probably, we will refactor both components into one.
					//
					


				}

			}



			return (scene) ? scene : nullptr;
		}




		OrthographicCamera::OrthographicCamera(CScene* scene, std::string tag) : CEntity(scene, scene->createEntity(tag)) {


		}




		// Camera has a "ComponentViewport" which holds
		// data about the viewport dimensions,
		// the aspectratio and camera height.
		// The height is stored there as we do not want to polute the 
		// "ComponentTransform" with a camera specific entry.
		bool OrthographicCamera::init(unsigned int viewportWidth, unsigned  int viewportHeight) {

			if (!this->hasComponent<ComponentTransform>()) {

				this->addComponent< ComponentTransform >();
				this->getComponent< ComponentTransform >().m_Position = glm::vec2(0.0f, 0.0f);
			}

			if (!this->hasComponent<ComponentClassName>()) {

				auto& className = this->addComponent<ComponentClassName>();
				className.m_ClassName = "OrthographicCamera";
			}


			if (!this->hasComponent<ComponentViewport>()) {

				auto& viewport = this->addComponent< ComponentViewport >();
				viewport.m_ViewportWidth = (float)viewportWidth;
				viewport.m_ViewportHeight = (float)viewportHeight;
				viewport.m_CameraHeight = 5.0f;
				viewport.m_AspectRatio = viewport.m_ViewportWidth / viewport.m_ViewportHeight;
			}

			m_viewportHeight = (float)viewportHeight;
			m_ViewportWidth = (float)viewportWidth;

			_recalculateProjection();

			return true;
		}



		void OrthographicCamera::move(glm::vec2 vec) {

			this->getComponent<ComponentTransform>().m_Position += vec;
		}


		void OrthographicCamera::move(float x, float y) {

			glm::vec2 vec = glm::vec2(x, y);
			this->getComponent<ComponentTransform>().m_Position += vec;
		}


		void OrthographicCamera::elevate(float z) {

			this->getComponent<ComponentViewport>().m_CameraHeight += z;
		}


		void OrthographicCamera::sink(float z) {

			this->getComponent<ComponentViewport>().m_CameraHeight -= z;
		}


		void OrthographicCamera::teleport(glm::vec2 vec) {

			this->getComponent<ComponentTransform>().m_Position = vec;
		}


		void OrthographicCamera::teleport(float x, float y) {

			glm::vec2 vec = glm::vec2(x, y);
			this->getComponent<ComponentTransform>().m_Position = vec;
		}


		void OrthographicCamera::elevateTeleport(float z) {

			this->getComponent<ComponentViewport>().m_CameraHeight = z;
		}




		glm::vec2 OrthographicCamera::getViewport() const {

			// Temporary solution...
			return glm::vec2(m_ViewportWidth, m_viewportHeight);

			//auto& cmp = this->getComponent<ComponentViewport>();

			//return glm::vec2(cmp.m_ViewportWidth, cmp.m_ViewportHeight);
		}


		glm::mat4 OrthographicCamera::_getLookAt() {


			glm::vec2 pos = this->getComponent<ComponentTransform>().m_Position;
			glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(pos, this->getComponent< ComponentViewport >().m_CameraHeight)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0, 0, 1));

			view = glm::inverse(view);

			return view;
		}


		glm::mat4 OrthographicCamera::_getProjection() {

			float cam_height = this->getComponent< ComponentViewport >().m_CameraHeight;
			float aspect_ratio = this->getComponent< ComponentViewport >().m_AspectRatio;


			float left, right, bottom, top;
			left = -cam_height;
			right = aspect_ratio * cam_height;
			bottom = -cam_height;
			top = cam_height;
			glm::mat4 proj = glm::ortho(left, right, bottom, top, -1.0f, 1000.0f);

			return proj;
		}



		void OrthographicCamera::_recalculateProjection() {

			m_ViewProjectionMatrix = _getProjection() * _getLookAt();
		}











		// Because of the entity, we need to reference a scene in the start.
		// We create an entity representing this system and holding all needed data.
		CParticleSystem::CParticleSystem(CScene* scene, std::string entityTag) : CEntity(scene, scene->createEntity(entityTag)) {

		}



		// As particles can have different textures, 
		// we need to specify it. Later we will request a handle to that texture in the RessourceManager.
		//
		// Further, we do not need to set a shader, as with batch drawing theres a default one.
		//
		// It is neeeded to set the definition for a particle.
		// Even if we deserialize, we only deserialize the data and create a new "CParticleSystem" from it.
		//
		bool CParticleSystem::init(std::string texturePath, ComponentParticleData* data, ComponentParticlePositionMode* positionMode, int particleCount) {

			// A particles system has
			// ComponentTexture2D for texture
			//
			// We have explicitly no components as transform, graphics etc.
			// as those data are set dynamically for each particle.
			//
			// But as for every sprite  we have a texture coordinates component.
			// Thus we even can set them dynamically combine anim.sprites with particles...
			ComponentMemoryProtocol2D* memory = nullptr;
			ComponentParticleData* pData = nullptr;
			ComponentParticlePositionMode* pMode = nullptr;
			ComponentTexture2D* pTexture = nullptr;


			if (!this->hasComponent<ComponentClassName>()) {
				auto& cmp = this->addComponent< ComponentClassName >();
				cmp.m_ClassName = "CParticleSystem";
			}


			if (this->hasComponent< ComponentMemoryProtocol2D >()) {

				memory = &this->getComponent< ComponentMemoryProtocol2D >();
			}
			else {
				memory = &this->addComponent<ComponentMemoryProtocol2D>();
			}
			memory->m_TextureCoords[0] = glm::vec2(0.0f, 0.0f);
			memory->m_TextureCoords[1] = glm::vec2(1.0f, 0.0f);
			memory->m_TextureCoords[2] = glm::vec2(1.0f, 1.0f);
			memory->m_TextureCoords[3] = glm::vec2(0.0f, 1.0f);



			if (this->hasComponent< ComponentTexture2D >()) {

				pTexture = &this->getComponent< ComponentTexture2D >();
			}
			else {
				pTexture = &this->addComponent<ComponentTexture2D>();
			}

			this->getComponent<ComponentTexture2D>().init(texturePath); // Load texture.





			if (this->hasComponent< ComponentParticleData >()) {

				pData = &this->getComponent< ComponentParticleData >();
			}
			else {
				pData = &this->addComponent<ComponentParticleData>();
			}

			if (this->hasComponent< ComponentParticlePositionMode >()) {

				pMode = &this->getComponent< ComponentParticlePositionMode >();
			}
			else {
				pMode = &this->addComponent< ComponentParticlePositionMode >();
			}

			// Copy data.
			pData->ColorStart = data->ColorStart;
			pData->ColorEnd = data->ColorEnd;
			pData->ColorStartVar = data->ColorStartVar;
			pData->ColorEndVar = data->ColorEndVar;
			pData->Position = data->Position;
			pData->Velocity = data->Velocity;
			pData->PositionVar = data->PositionVar;
			pData->VelocityVar = data->VelocityVar;
			pData->EmissionRate = data->EmissionRate;
			pData->MaxLifetime = data->MaxLifetime;
			pData->SizeStart = data->SizeStart;
			pData->SizeEnd = data->SizeEnd;
			pData->Rotation = data->Rotation;
			pData->RotationSpeed = data->RotationSpeed;
			pData->RotationDir = data->RotationDir;
			pData->MaxLifeTimeVar = data->MaxLifeTimeVar;
			pData->SizeStartVar = data->SizeStartVar;
			pData->SizeEndVar = data->SizeEndVar;
			pData->RotationVar = data->RotationVar;
			pData->RotationSpeedVar = data->RotationSpeedVar;
			pData->SpeedVar = data->SpeedVar;
			pData->Speed = data->Speed;
			pData->ParticleCount = data->ParticleCount;

			switch (positionMode->m_PositionMode) {
			case ComponentParticlePositionMode::Mode::Following_Entity:
				pMode->m_PositionMode = ComponentParticlePositionMode::Mode::Following_Entity;

				if (!pMode->m_Mode_Following_Entity) {

					pMode->m_Mode_Following_Entity = new ComponentParticlePositionMode::Mode_Following_Entity();
					pMode->m_Mode_Following_Entity->EntityHandle = positionMode->m_Mode_Following_Entity->EntityHandle;
				}
				break;


			case ComponentParticlePositionMode::Mode::Fixed_To_Position:
				pMode->m_PositionMode = ComponentParticlePositionMode::Mode::Fixed_To_Position;

				if (!pMode->m_Mode_Fixed_To_Position) {

					pMode->m_Mode_Fixed_To_Position = new ComponentParticlePositionMode::Mode_Fixed_To_Position();
					pMode->m_Mode_Fixed_To_Position->ModePosition = positionMode->m_Mode_Fixed_To_Position->ModePosition;
				}
				break;


			case ComponentParticlePositionMode::Mode::Fixed_To_Space:
				pMode->m_PositionMode = ComponentParticlePositionMode::Mode::Fixed_To_Space;

				if (!pMode->m_Mode_Fixed_To_Space) {

					pMode->m_Mode_Fixed_To_Space = new ComponentParticlePositionMode::Mode_Fixed_To_Space();
					pMode->m_Mode_Fixed_To_Space->Point = positionMode->m_Mode_Fixed_To_Space->Point;
					pMode->m_Mode_Fixed_To_Space->RectHeight = positionMode->m_Mode_Fixed_To_Space->RectHeight;
					pMode->m_Mode_Fixed_To_Space->RectWidth = positionMode->m_Mode_Fixed_To_Space->RectWidth;
				}
				break;


			default:
				return false;
			}


			if (!hasComponent< ComponentParticle >()) {
				auto& pool = addComponent< ComponentParticle >();
				
				pool.IsActive = false;
				pool.ParticleColorEnd = glm::vec4(0.0f);
				pool.ParticleColorStart = glm::vec4(0.0f);
				pool.ParticleLifetime = 0.0f;
				pool.ParticlePosition = glm::vec2(0.0f);
				pool.ParticleRotation = 0.0f;
				pool.ParticleRotationDir = 0;
				pool.ParticleRotationSpeed = 0.0f;
				pool.ParticleSizeEnd = 0.0f;
				pool.ParticleSizeStart = 0.0f;
				pool.ParticleSpeed = 0.0f;
				pool.ParticleVelocity = glm::vec2(0.0f);
			}

			if (!hasComponent<ComponentParticlePool>()) {

				auto& pool = addComponent< ComponentParticlePool >();
				pool.m_CurrentParticleIndex = 0;
				pool.m_ParticlePool.resize(particleCount);
			}


			return true;
		}


		// Function emit given amount of particles. The amount is set in ParticleData..
		// Emitting means, setting some particles as active, and thus, drawable.
		// 
		// Further we set the data for the activated particle as defined in  "ComponentParticleData"
		// and based on positioning mode as well as randomiztion data...
		void CParticleSystem::emit() {


			auto& pData = getComponent<ComponentParticleData>();
			auto& pMode = getComponent<ComponentParticlePositionMode>();
			auto& pool = getComponent< ComponentParticlePool >();


			// Emitt defined count of particles.
			for (int i = 0; i < pData.EmissionRate; i++) {


				ComponentParticle& particle = pool.m_ParticlePool[pool.m_CurrentParticleIndex];
				particle.IsActive = true;



				// set data based on data and randomizations...
				if (pMode.m_PositionMode == ComponentParticlePositionMode::Mode::Fixed_To_Position) {

					// Give randomized position for particle.
					particle.ParticlePosition = pMode.m_Mode_Fixed_To_Position->ModePosition + Random::Float() * Random::AlternatingOne() * pData.PositionVar;

				}
				else if (pMode.m_PositionMode == ComponentParticlePositionMode::Mode::Fixed_To_Space) {

					// Give random position in between the point and defined rectangle.
					particle.ParticlePosition = glm::vec2(pMode.m_Mode_Fixed_To_Space->Point.x + Random::Float() * pData.PositionVar.x * pMode.m_Mode_Fixed_To_Space->RectWidth,
						pMode.m_Mode_Fixed_To_Space->Point.y + Random::Float() * pData.PositionVar.y * pMode.m_Mode_Fixed_To_Space->RectHeight);

				}
				else if (pMode.m_PositionMode == ComponentParticlePositionMode::Mode::Following_Entity) {

					// Give randomized position around the entities position.
					CEntity* entity = m_Scene->getEntity(pMode.m_Mode_Following_Entity->EntityHandle).get();
					pData.Position = entity->getComponent<ComponentTransform>().m_Position;

					particle.ParticlePosition = pData.Position + Random::Float() * Random::AlternatingOne() * pData.PositionVar;

				}
				else {

					// No valid mode set, do nothing...
					particle.IsActive = false;
					return;
				}


				// Set the rotation, velocity etc.
				particle.ParticleRotation = pData.Rotation + Random::AlternatingOne() * pData.RotationDir * pData.RotationVar;
				particle.ParticleRotationSpeed = pData.RotationSpeed + Random::AlternatingOne() * pData.RotationSpeedVar;


				particle.ParticleColorStart = glm::clamp((pData.ColorStart + Random::Float() * Random::AlternatingOne() * pData.ColorStartVar), glm::vec4(0.1f), glm::vec4(1.0f));
				particle.ParticleColorEnd = glm::clamp((pData.ColorEnd + Random::Float() * Random::AlternatingOne() * pData.ColorEndVar), glm::vec4(0.1f), glm::vec4(1.0f));


				// Lifetime is clamped between 2 times the max lifetime and the minimal possible, 0.1
				particle.ParticleLifetime = glm::clamp((pData.MaxLifetime + Random::Float() * Random::AlternatingOne() * pData.MaxLifeTimeVar), 0.1f, pData.MaxLifetime * 2);
				particle.ParticleMaxLifetime = particle.ParticleLifetime;


				particle.ParticleSizeStart = glm::clamp((pData.SizeStart + Random::Float() * Random::AlternatingOne() * pData.SizeStartVar), 0.001f, pData.SizeStart * 2);
				particle.ParticleSizeEnd = glm::clamp((pData.SizeEnd + Random::Float() * Random::AlternatingOne() * pData.SizeEndVar), 0.001f, pData.SizeEnd * 2);


				particle.ParticleVelocity = pData.Velocity + Random::AlternatingOne() * pData.VelocityVar;

				particle.ParticleSpeed = pData.Speed + Random::AlternatingOne() * pData.SpeedVar;


				pool.m_CurrentParticleIndex = (++pool.m_CurrentParticleIndex) % pool.m_ParticlePool.size();
			}



		}


		// This function is intended to be called in each renderloop.
		//
		// Here we specify the data of active particles based on the lifetime and
		// the elapsed time.
		//
		// Further we call the Renderer::Draw() function here to draw the particles,
		// so user can choose here the drawing "layer".
		void CParticleSystem::onRender(float dt) {

			auto& pool = getComponent< ComponentParticlePool >();


			// Update the data of ctive particles and set them to be drawn.
			for (ComponentParticle& particle : pool.m_ParticlePool) {

				if (!particle.IsActive) continue;

				if (particle.ParticleLifetime <= 0.0f) {

					particle.IsActive = false;
				}


				particle.ParticleLifetime -= dt;


				particle.ParticlePosition += particle.ParticleSpeed * particle.ParticleVelocity * (float)dt;


				particle.ParticleRotation += particle.ParticleRotationSpeed + particle.ParticleRotationDir * dt;


				float life = particle.ParticleLifetime / particle.ParticleMaxLifetime;

				// Define color based on lifetime,
				// and clamp the value to min of 0.01f and max of 1.0f. Thus it cant be invisible...
				glm::vec4 color;
				color.r = common_lerp(particle.ParticleColorEnd.r, particle.ParticleColorStart.r, life);
				color.g = common_lerp(particle.ParticleColorEnd.g, particle.ParticleColorStart.g, life);
				color.b = common_lerp(particle.ParticleColorEnd.b, particle.ParticleColorStart.b, life);
				color.a = common_lerp(particle.ParticleColorEnd.a, particle.ParticleColorStart.a, life);

				float size = common_lerp(particle.ParticleSizeEnd, particle.ParticleSizeStart, life);


				// Compute model transform and other data...
				glm::mat4 model_transform = glm::translate(glm::mat4(1.0f), glm::vec3(particle.ParticlePosition, 1.0f));
				model_transform *= glm::rotate(glm::mat4(1.0f), particle.ParticleRotation, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(size, size, 1.0f));

				ComponentMemoryProtocol2D* memoryProtocol = &this->getComponent<ComponentMemoryProtocol2D>();
				ComponentTexture2D* texture = &this->getComponent<ComponentTexture2D>();


				// Call renderer here...
				BatchRenderer2D::draw(memoryProtocol, model_transform, texture, color);
			}


		}

















	}


}




namespace nautilus {

	namespace graphics {


		// Create a sprite dedicated to given scene.
		CSprite::CSprite(CScene* scene, std::string entityTag) : CEntity(scene, scene->createEntity(entityTag)) {

		}



		// Initialize components, texture and shader.
		// We do not set the drawing layer here explicitly.
		// As we let for now the user decide dynamically to which "layer" to draw,
		// what can be easily accomplished.
		bool CSprite::init(std::string textureName) {


			// What does a sprite has?
			// ComponentMemoryProtocol2D for texture coords.
			// ComponentGraphics for color.
			// ComponentTransform for position, scale and rotation.
			// 
			// ComponentTexture2D as handle to the texture.

			if (!this->hasComponent< ComponentTransform >()) {

				this->addComponent<ComponentTransform>();
			}

			if (!this->hasComponent< ComponentMemoryProtocol2D >()) {

				auto& memory = this->addComponent<ComponentMemoryProtocol2D>();
				memory.m_TextureCoords[0] = glm::vec2(0.0f, 0.0f);
				memory.m_TextureCoords[1] = glm::vec2(1.0f, 0.0f);
				memory.m_TextureCoords[2] = glm::vec2(1.0f, 1.0f);
				memory.m_TextureCoords[3] = glm::vec2(0.0f, 1.0f);
			}

			if (!this->hasComponent< ComponentGraphics >()) {

				this->addComponent<ComponentGraphics>();
			}

			if (!this->hasComponent< ComponentTexture2D >()) {

				auto& texture = this->addComponent<ComponentTexture2D>();
				texture.init(textureName);
			}

			if (!this->hasComponent< ComponentClassName >()) {

				auto& classname = this->addComponent<ComponentClassName>();
				classname.m_ClassName = "CSprite";
			}


			return true;
		}


		// Common sprite functions.
		void CSprite::setPosition(glm::vec2 vec) {

			this->getComponent<ComponentTransform>().m_Position = vec;
		}

		void CSprite::setScale(glm::vec2 vec) {

			this->getComponent<ComponentTransform>().m_Scale = vec;
		}


		void CSprite::setColor(glm::vec4 vec) {

			this->getComponent<ComponentGraphics>().m_Color = vec;
		}


		void CSprite::setRotation(float angle) {

			this->getComponent<ComponentTransform>().m_Rotation = angle;
		}


		void CSprite::setBrightness(float n) {

			auto& cmp = this->getComponent<ComponentGraphics>().m_Color;
			cmp.a = glm::clamp(n, 0.01f, 1.0f);
		}


	}

}







namespace nautilus {

	namespace graphics {

		// Create a sprite dedicated to given scene.
		CAnimatedSprite::CAnimatedSprite(CScene* scene, std::string entityTag) : CEntity(scene, scene->createEntity(entityTag)) {

		}

		// Initialize components, texture and shader.
		// We do not set the drawing layer here explicitly.
		// As we let for now the user decide dynamically to which "layer" to draw,
		// what can be easily accomplished.
		//
		// As we will use this sprite in the batch shader,
		// we do not set the shader here,
		// the shader is the same for all and set in the renderer.
		bool CAnimatedSprite::init(std::string textureName, int textureRows, int textureColumns, float playSpeed) {

			// What does an anim. sprite has?

			// ComponentMemoryProtocol2D for texture coords. Very needed for animation.
			// ComponentGraphics for color.
			// ComponentTransform for the position, scale...
			//
			// For now, same as in CSprite, we have the texture component here.
			// Later we will get the texture handle from the RessourceManager.
			// ComponentTexture2D for the actual texture. 

			glm::vec2 size = glm::vec2(0.0f);

			if (!hasComponent<ComponentTransform>()) {

				auto& transform = addComponent<ComponentTransform>();

			}


			if (!hasComponent<ComponentMemoryProtocol2D>()) {

				auto& memory = addComponent<ComponentMemoryProtocol2D>();
				memory.m_TextureCoords[0] = glm::vec2(0.0f, 0.0f);
				memory.m_TextureCoords[1] = glm::vec2(1.0f, 0.0f);
				memory.m_TextureCoords[2] = glm::vec2(1.0f, 1.0f);
				memory.m_TextureCoords[3] = glm::vec2(0.0f, 1.0f);
			}


			if (!hasComponent<ComponentGraphics>()) {

				auto& graphics = addComponent<ComponentGraphics>();
				graphics.m_Color = glm::vec4(1.0f);
			}



			if (!hasComponent<ComponentTexture2D>()) {

				auto& texture = addComponent<ComponentTexture2D>();
				texture.init(textureName);
				size = texture.GetSize();
			}
			else {
				size = getComponent< ComponentTexture2D >().GetSize();
			}



			if (!hasComponent<ComponentAnimationData>()) {

				auto& animationData = addComponent<ComponentAnimationData>();
				animationData.m_AnimationColumns = textureColumns;
				animationData.m_AnimationRows = textureRows;
				animationData.m_PlaySpeed = playSpeed;
				animationData.m_TextureWidth = size.x;
				animationData.m_TextureHeight = size.y;
				animationData.m_FrameWidth = animationData.m_TextureWidth / animationData.m_AnimationRows;
				animationData.m_FrameHeight = animationData.m_TextureHeight / animationData.m_AnimationColumns;
				animationData.m_CurrentFrameX = 0;
				animationData.m_CurrentFrameY = 0;
				animationData.m_AnimationCursor = 0.0f;
			}


			if (!hasComponent<ComponentClassName>()) {

				auto& className = this->addComponent<ComponentClassName>();
				className.m_ClassName = "CAnimatedSprite";
			}



			return true;
		}



		// Function or advancing the animation cursor and
		// determining whether to set a "new frame" as current sprite.
		void CAnimatedSprite::play(float dt) {



			float sheetwidth = 0;
			float sheetheight = 0;
			float framewidth = 0;
			float frameheight = 0;


			auto& animationData = this->getComponent<ComponentAnimationData>();

			// Advance the animation cursor as mean of current frame time and 
			// user defined animation speed.
			animationData.m_AnimationCursor += (dt + animationData.m_PlaySpeed) / 2.0f;



			// Calculate for setting texture coordinates.
			sheetwidth = animationData.m_TextureWidth;
			sheetheight = animationData.m_TextureHeight;
			framewidth = sheetwidth / animationData.m_AnimationRows;
			frameheight = sheetheight / animationData.m_AnimationColumns;


			// Check for frame transition
			if (animationData.m_AnimationCursor > 1.0f) {


				// Set next frame indeces.
				animationData.m_CurrentFrameX = (animationData.m_CurrentFrameX + 1) % animationData.m_AnimationRows;

				// Recalculate the column index only, if we reached the end of the row..
				if (animationData.m_CurrentFrameX == 0) {

					animationData.m_CurrentFrameY = (animationData.m_CurrentFrameY + 1) % animationData.m_AnimationColumns;
				}

				animationData.m_AnimationCursor = 0;
			}



			// Set new texture coordinates.
			auto& memoryProtocol = this->getComponent<ComponentMemoryProtocol2D>();
			memoryProtocol.m_TextureCoords[0] = glm::vec2((animationData.m_CurrentFrameX * framewidth) / sheetwidth, (animationData.m_CurrentFrameY * frameheight) / sheetheight);
			memoryProtocol.m_TextureCoords[1] = glm::vec2(((animationData.m_CurrentFrameX + 1) * framewidth) / sheetwidth, (animationData.m_CurrentFrameY * frameheight) / sheetheight);
			memoryProtocol.m_TextureCoords[2] = glm::vec2(((animationData.m_CurrentFrameX + 1) * framewidth) / sheetwidth, ((animationData.m_CurrentFrameY + 1) * frameheight) / sheetheight);
			memoryProtocol.m_TextureCoords[3] = glm::vec2((animationData.m_CurrentFrameX * framewidth) / sheetwidth, ((animationData.m_CurrentFrameY + 1) * frameheight) / sheetheight);

		}


		// Common sprite functions.
		void CAnimatedSprite::setPosition(glm::vec2 vec) {

			this->getComponent<ComponentTransform>().m_Position = vec;
		}

		void CAnimatedSprite::setScale(glm::vec2 vec) {

			this->getComponent<ComponentTransform>().m_Scale = vec;
		}


		void CAnimatedSprite::setColor(glm::vec4 vec) {

			this->getComponent<ComponentGraphics>().m_Color = vec;
		}


		void CAnimatedSprite::setRotation(float angle) {

			this->getComponent<ComponentTransform>().m_Rotation = angle;
		}


		void CAnimatedSprite::setBrightness(float n) {

			auto& cmp = this->getComponent<ComponentGraphics>().m_Color;
			cmp.a = glm::clamp(n, 0.01f, 1.0f);
		}



	}


}