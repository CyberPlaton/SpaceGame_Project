#pragma once

#include"Base.h"
#include"Component.h"


#include<array>


namespace nautilus {

	namespace graphics {


		struct QuadVertex {
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TextureCoords;
			float TextureIndex;
		};




		class QuadIndexBuffer {
		public:
			// Create index buffer from define indices,
			// they will stay the same.
			// Here we take in a pointer to first indices element and
			// the needed size of the buffer.
			QuadIndexBuffer(GLuint* indices, int size);
			~QuadIndexBuffer();



			// Every buffer should be able to be bound and
			// unbound.
			void bind();
			void unbind();

			int getIndexCount() const { return m_IndicesCount; }

		private:

			// Each buffer gets an ID on creation.
			// Thus to bind a certain buffer or to set its data, we needs its ID,
			// which we save here.
			GLuint m_RendererID;


			int m_IndicesCount = 0;

		private:


		};




		class QuadVertexBuffer {
		public:

			// A quad vertex buffer takes in only the size
			// of the buffer to be created.
			// Later, we set dynamically the vertices, that is data.
			QuadVertexBuffer(int size);
			~QuadVertexBuffer();



			// Set the data of this buffer.
			// Intended to be set every flush frame.
			void setBufferData(const void* data, int size);


			// Every buffer should be able to be bound and
			// unbound.
			void bind();
			void unbind();


		private:

			// Each buffer gets an ID on creation.
			// Thus to bind a certain buffer or to set its data, we needs its ID,
			// which we save here.
			GLuint m_RendererID;

		private:

		};





		// The vertex array is just a storage for vertex buffers and 
		// an index buffer.
		//
		// Further we use it for drawing (get current index count).
		class QuadVertexArray {
		public:


			QuadVertexArray();
			~QuadVertexArray();


			void addVertexBuffer(QuadVertexBuffer* vertexBuffer);
			void setIndexBuffer(QuadIndexBuffer* indexBuffer);


			void bind();
			void unbind();

			int getIndicesCount() const { return m_IndexBuffer->getIndexCount(); }


		private:

			GLuint m_RendererID; // For glBindVertexArray



			// Our storage of Vertexbuffers.
			std::vector<QuadVertexBuffer*> m_VertexBuffers;
			QuadIndexBuffer* m_IndexBuffer;


			GLuint m_CurrentVertexBufferIndex; // To enable the buffer and setting the attributes.
												/*
												glBindVertexArray( m_RendererID ) // Bind this vertex array

												glBindBuffer( buffers renderer ID )

												glEnableVertexAttribArray(m_CurrentVertexBufferIndex) // Set layout for buffer with some index
												glVertexAttribPointer( layout data ). // Ande set the actual layout



												The layout must be enable every time we add a vertex buffer.



												*/


		private:



			// For each vertex buffer we set predefined layout.
			// We have thi function in vertex array, as we must bind the vertex array dynamically
			// to set the active layout for the vertex buffer.
			//
			// This function must be called on each vertex buffer we add.
			void _setBufferLayout();
		};






		// Holding data needed for seemless batch rendering,
		// likewise here can be defined some stats for rendering,
		// like max. count of vertices or textures etc.
		// Here too we store pointers to currently used vertices (begin and end).
		struct RenderData2D {


			int maxQuads = 20000;
			int maxVerts = 4 * maxQuads;
			int maxIndices = 6 * maxQuads;
			int maxTextures = 32; // Needed for shader, as we bind textures to explicit slots and access them in the shader.



			// The array for this scene/batch.
			QuadVertexArray* m_BatchVertexArray;



			// The actuall batch vertex buffer.
			// Is for setting the data, and flushing.
			QuadVertexBuffer* m_BatchVertexBuffer;



			// We need a pointer to a batch shader.
			// The shader is for all elements rendered the same...
			//
			// For this we can take ComponentShader
			// as it has all functionality we need...
			//
			// ComponentShader* m_BatchShader; // This shader will be bound on each draw call.
			static nautilus::graphics::ComponentShader* m_BatchShader;



			// Furthermore, we need to set the right textures in the correct Opengl slots.
			// And we need to store them.
			// For the texture we can take ComponentTexture2D,
			// as it has all data and members needed.
			//
			// 
			// ComponentTexture2D* m_WhiteTexture; // Texture for binding as default.
			nautilus::graphics::ComponentTexture2D* m_WhiteTexture;
			std::array<nautilus::graphics::ComponentTexture2D*, 32> m_TextureSlots;
			int m_TextureSlotIndex = 1; // First ( = 0 ) is the default white texture.
									// Here we keep count of currently set textures for drawing.
									// A shader can take as input max 32 textures.


			/*
			To get the size of the of the area defined by the first and last vertex,
			use:

			// Compute the bytes between first and last vertex.
			(uint_8*)m_QuadVertexEnd - (uint_8*)m_QuadVertexBegin

			// better cast it to what the function "setBufferData" takes as size input.
			and cast this to (uint_32).
			*/
			QuadVertex* m_QuadVertexBegin = nullptr; // First vertex.
			QuadVertex* m_QuadVertexEnd = nullptr; // Last vertex.


			int m_QuadIndexCount = 0;


			// Standard vertex positions for all quads.
			// As we do not chnage them, they can be set from here...
			glm::vec3 m_QuadVertexPositions[4] = {
			glm::vec3(-0.5f, -0.5f, 0.0f),
			glm::vec3(0.5f, -0.5f, 0.0f),
			glm::vec3(0.5f, 0.5f, 0.0f),
			glm::vec3(-0.5f, 0.5f, 0.0f)
			};
		};





		// Static renderer class.
		// One per scene/running application scene.
		//
		// Takes in draw calls and sets the data into the vertex buffers,
		// at the end flushes the buffers to gpu as a drawcall.
		//
		// We flush and restart a batch, if buffer max size was reached.
		// Thus we minimize draw calls and maximie FPS...
		class BatchRenderer2D {
		public:

			// Functions called on app start and end.
			static void init(); // Allocated batch buffer and indices, set up batch and prepare for drawing.
			static void shutDown(); // Destroy buffers and deallocate all data.


			// Functions must be called on begin of each "scene",
			// means before we call draw functions, we call beginScene, and as we are done,
			// we call endScene.
			static void beginScene(glm::mat4 view_projection); // Function needed to be called on starting drawing a batch.
									  // Bind the batches shader and set uniform, start batch.

			static void endScene(); // Flushes the batch data, means, make a draw call and draw everything currently in batch buffer.


			// To draw, we only need to set the transform of the model,
			// which is translated, rotated and scaled...
			// Furthermore the actual texture for setting it in the appropriate slot.
			// And a color, if we want to...
			static void draw(ComponentMemoryProtocol2D* memoryProtocol, glm::mat4 model_transform, nautilus::graphics::ComponentTexture2D* texture, glm::vec4 color = glm::vec4(1.0f));


		private:


		private:

			static void _startBatch(); // Begin a new Batch, for it we must flush the current,
									   // means, we draw directly what is in right now and begin setting draw data anew.

			static void _nextBatch(); // Convenience. Flush batch and start new one.


			static void _flush(); // Actually sending draw data to GPU. Draw everything currently in buffer.

		};


	}

}
