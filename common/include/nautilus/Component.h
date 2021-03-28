#pragma once

#include"Base.h"
#include"Scripting.h"

namespace nautilus {

	namespace graphics {


		struct ComponentViewport {

			float m_AspectRatio;
			float m_ViewportWidth;
			float m_ViewportHeight;
			float m_CameraHeight;

		};


		struct ComponentID {
			std::string m_Tag;
			uint32_t m_ID;
		};


		struct ComponentClassName {
			std::string m_ClassName;
		};


		struct ComponentTransform {

			glm::vec2 m_Position = {0.0f, 0.0f};
			glm::vec2 m_Scale = {1.0f, 1.0f};
			
			float m_Rotation = 0.0f;
		};




		struct ComponentGraphics {

			glm::vec4 m_Color = {1.0f, 1.0f, 1.0f, 1.0f};
		};



		// This component is never serialized or deserialized,
		// it is merely used as interface...
		//
		struct ComponentFileResource {

			std::string m_FilePath;
		};





		class ComponentTexture2D : public ComponentFileResource {
		public:

			ComponentTexture2D() = default;
			virtual ~ComponentTexture2D();

			void init(std::string filename);


			bool LoadTexture(const std::string& fileName, bool genMipMaps = true);
			void Bind(GLuint texUint = 0);
			void BindForBatch(GLuint texUint = 0);

			void Unbind(GLuint texUint = 0);
			GLuint GetSlot() const { return m_TextureHandle; }



			glm::vec2 GetSize()const { return m_Size; }
			std::string GetPath()const { return m_FilePath; }

		private:

			GLuint m_TextureHandle;

			glm::vec2 m_Size = glm::vec2(0.0f);
		};





		class ComponentShader : public ComponentFileResource {
		public:
			enum class ShaderType {
				SHADER_TYPE_VERTEXSHADER,
				SHADER_TYPE_FRAGMENTSHADER,
				SHADER_TYPE_SHADERPROGRAM
			};


		public:
			ComponentShader() = default;
			virtual ~ComponentShader();

			void init(std::string filename);

			bool LoadShaders(const char* vsFilename, const char* fsFilename);

			void Use();


			// For shader static variables.
			void SetUniform(const GLchar* name, const glm::vec2& v);
			void SetUniform(const GLchar* name, const glm::vec3& v);
			void SetUniform(const GLchar* name, const glm::vec4& v);

			void SetUniform(const GLchar* name, const glm::mat4& m); // For matrices.

			void SetUniform(const GLchar* name, const GLfloat& f);
			void SetUniform(const GLchar* name, const GLint& i);

			void SetUniformArray(const GLchar* name, const GLint* values, int count);

			void SetUniformSampler(const GLchar* name, const GLint& textureSlot);


			GLuint GetProgram()const { return m_ProgramHandle; }

		private:

			// Map for storing uniforms.
			// Locations shall not change during program.
			std::map<std::string, GLint> m_UniformLocationsMap;

			GLuint m_ProgramHandle = NULL;

		private:

			std::string _fileToString(const std::string& filename);
			void _compilingCheck(GLuint shaderObj, ShaderType sType);

			// Utility for uniform variables.
			GLint _getUniformLocation(const GLchar* name);

		};




		class ComponentScript{
		public:
			ComponentScript() = default;

			void init(std::string filepath) {m_Script = CreateScope<nautilus::core::Script>(filepath);}

			bool executeScript() { return nautilus::core::LuaBinding::execute(*m_Script.get());}


			Scope<nautilus::core::Script> m_Script;
		};



		struct ComponentRenderableEntity {
			bool m_IsRenderable = true;
			bool m_IsAnimated = false;
		};




		// New struct for batch rendering.
		//
		// Defines the texture coordinates for an entity.
		//
		// We use this as we render and set the data dynamically...
		struct ComponentMemoryProtocol2D {

			glm::vec2 m_TextureCoords[4] = {
					glm::vec2(0.0f),
					glm::vec2(0.0f),
					glm::vec2(0.0f),
					glm::vec2(0.0f)
			};

		};



		/*
		Struct for serializing data we set for an anim. sprite.

		With this we can export it to file and load dynamically
		a preset.

		E.g. on loading a scene which has anim. sprites...

		As these are all floats/ints,
		we do not need to have it in "Scene.h", thus we can have "yaml.h" in "Components.h".
		*/
		struct ComponentAnimationData {

			float m_TextureWidth = 0;
			float m_TextureHeight = 0;
			float m_FrameWidth = 0;
			float m_FrameHeight = 0;
			float m_PlaySpeed = 0;
			float m_AnimationCursor = 0;

			int m_CurrentFrameX = 0;
			int m_CurrentFrameY = 0;

			// A animation atlas is actually like a n*m-Matrix.
			int m_AnimationRows = 0;
			int m_AnimationColumns = 0;
		};







		// This component stores all the data needed
		// for emitting and managing the particles 
		// of the associated particle system.
		//
		// It is intended to be in "Component.h" and will be serializable.
		//
		// By design, it should be possible to randomize every part of the data
		// by the amount a user/designer wants to.
		// Thus, we have for "everything" a variation entry..
		//
		//
		// On serialize, we export this data.
		// On deserialize, we import the data and create a "ComponentParticleData" object,
		// this and the scene we use to initialize a particle system.
		//
		struct ComponentParticleData {

			// ParticleSystem related Data.
			float EmissionRate; // How many particles are emitted on each update cycle.
			unsigned int ParticleCount;

			// Base data.
			glm::vec2 Position; // The position.
			glm::vec2 Velocity; // Movement direction.

			glm::vec4 ColorStart; // Color at life begin
			glm::vec4 ColorEnd; // Color at life end

			float MaxLifetime; // Maximal tim to live for a particle
			float SizeStart; // Size at life start
			float SizeEnd; // Size at life end

			float Rotation; // At which angle the texture is rotated
			int RotationDir; // The direction in which the texture rotates
			float RotationSpeed; // The speed of rotation

			float Speed; // The speed with which the particle moves in certain direction


			// Randomizing data.
			glm::vec2 PositionVar;
			glm::vec2 VelocityVar;

			glm::vec4 ColorStartVar;
			glm::vec4 ColorEndVar;

			float MaxLifeTimeVar;
			float SizeStartVar;
			float SizeEndVar;

			float RotationVar;
			float RotationSpeedVar;

			float SpeedVar;
		};







		// Furthermore,
		// we want to be able to set te position in relation to some 2D space in the game.
		//
		// Thus, we should be able to allow 3 different position modes:
		// 1) Mode to stay at the position specified in "ComponentParticleData"
		// 2) Mode to stay at the position of an entity. Here we should be able to set the entity handle,
		//      so that the system updates its data on evry loop..
		// 3) Mode to spawn particles in a predefined rectangle space. For it we can have the  
		//      "ComponentParticleData".Position as the point and a width for the width of the rect and height for the height of the rect.
		//
		// In addition, this data should be serializable and deserializable. Thus we make a component for it.
		//
		struct ComponentParticlePositionMode {

			enum class Mode {
				Invalid = -1,
				Fixed_To_Position = 0, // x, y Position
				Following_Entity = 1, // Entities x, y Position
				Fixed_To_Space = 2 // Rectangle
			};



			// We define nested structures for each mode.
			// In them we set needed data,
			// which in turn we use for exporting and importing.
			//
			// With the enum "Mode" we can dynamically decide what to import/export.
			struct Mode_Fixed_To_Position {

				glm::vec2 ModePosition;
			};


			// In order for this to work, 
			// the entity must have a "ComponentTransform" from
			// which we can get the position.
			//
			// Further we take an enitity pointer so we do not have to have a 
			// refrence to Scene or SceneManager to get the entity from a handle...
			//
			// Further this will be just faster...
			//
			// Note: 
			// we store the handle to the entity as a string, as with this we can be sure
			// that after serialization and deserialization
			// we still follow the same entity, as the entt::handle maybe change,
			// but the tag given by user "changes not"...
			//
			struct Mode_Following_Entity {

				std::string EntityHandle;
			};

			struct Mode_Fixed_To_Space {

				// We use as point := "ComponentParticleData".Position;
				// 
				// Note: 
				//
				// Due to serialization and deserialization, we want to have the Point in this struct.
				//
				glm::vec2 Point;
				float RectWidth;
				float RectHeight;
			};


			Mode m_PositionMode = Mode::Invalid;
			Mode_Fixed_To_Position* m_Mode_Fixed_To_Position = nullptr;
			Mode_Following_Entity* m_Mode_Following_Entity = nullptr;
			Mode_Fixed_To_Space* m_Mode_Fixed_To_Space = nullptr;


		};







		// Export the Particles from "CParticleSystem"
		// to a component, so that it becomes manageable by current engine.
		// Means we can easily static cast an entity to CParticleSystem and use its functionality
		// to emit and render particles...
		//
		// This component is HUGE, as a ParticlePool can be 1000 in size or larger,
		// thus we DONT want to export OR import it, merely dynamically create it for particle systems.
		//
		// More details in "CParticleSystem".
		//
		struct ComponentParticle {

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


		struct ComponentParticlePool {

			std::vector<ComponentParticle> m_ParticlePool;
			int m_CurrentParticleIndex = 0;
		};


	}

}


