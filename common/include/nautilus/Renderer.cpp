#include"Renderer.h"


namespace nautilus {

	namespace graphics {



		static RenderData2D* g_pRenderData2D = new RenderData2D();
		nautilus::graphics::ComponentShader* RenderData2D::m_BatchShader = nullptr;


		QuadVertexBuffer::QuadVertexBuffer(int size) : m_RendererID(0) {

			glGenBuffers(1, &m_RendererID);
			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
			glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); // Create buffer for dynamic drawing, we will change the data often on render.
		}



		QuadVertexBuffer::~QuadVertexBuffer() {

			glDeleteBuffers(1, &m_RendererID);
		}



		// Set the data of this buffer.
		// Intended to be set every flush frame.
		void QuadVertexBuffer::setBufferData(const void* data, int size) {

			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}



		// Every buffer should be able to be bound and
		// unbound.
		void QuadVertexBuffer::bind() {

			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		}

		void QuadVertexBuffer::unbind() {

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}


		// Create index buffer from define indices,
		// they will stay the same.
		// Here we take in a pointer to first indices element and
		// the needed size of the buffer.
		QuadIndexBuffer::QuadIndexBuffer(GLuint* indices, int size) : m_IndicesCount(size) {


			glGenBuffers(1, &m_RendererID);


			// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
			// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
			glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLuint), indices, GL_STATIC_DRAW); // The data is set once on BatchRenderer init and not changed...
		}



		QuadIndexBuffer::~QuadIndexBuffer() {

			glDeleteBuffers(1, &m_RendererID);
		}


		// Every buffer should be able to be bound and
		// unbound.
		void QuadIndexBuffer::bind() {

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

		}

		void QuadIndexBuffer::unbind() {

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		}




		QuadVertexArray::QuadVertexArray() {

			glGenVertexArrays(1, &m_RendererID);
		}

		QuadVertexArray::~QuadVertexArray() {

			glDeleteVertexArrays(1, &m_RendererID);
		}


		void QuadVertexArray::addVertexBuffer(QuadVertexBuffer* vertexBuffer) {



			glBindVertexArray(m_RendererID);
			vertexBuffer->bind();


			_setBufferLayout();


			m_VertexBuffers.push_back(vertexBuffer);
		}


		void QuadVertexArray::setIndexBuffer(QuadIndexBuffer* indexBuffer) {

			glBindVertexArray(m_RendererID);

			indexBuffer->bind();

			m_IndexBuffer = indexBuffer;
		}


		void QuadVertexArray::_setBufferLayout() { // Before call this function, bind the vertex array for which we set the layout.



			// Vertex Positions.
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (GLvoid*)offsetof(QuadVertex, Position));

			// Vertex colors.
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (GLvoid*)offsetof(QuadVertex, Color));

			// Vertex texture coords.
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (GLvoid*)offsetof(QuadVertex, TextureCoords));


			// Index of the texture we have bound.
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (GLvoid*)offsetof(QuadVertex, TextureIndex));

		}


		void QuadVertexArray::bind() {

			glBindVertexArray(m_RendererID);
		}


		void QuadVertexArray::unbind() {

			glBindVertexArray(0);
		}




		void BatchRenderer2D::draw(ComponentMemoryProtocol2D* memoryProtocol, glm::mat4 model_transform, nautilus::graphics::ComponentTexture2D* texture, glm::vec4 color) {

			using namespace std;


			int vertexCount = 4;
			glm::vec2 texCoords[] = { memoryProtocol->m_TextureCoords[0],
										memoryProtocol->m_TextureCoords[1],
										memoryProtocol->m_TextureCoords[2],
										memoryProtocol->m_TextureCoords[3]
			};


			// Set texture to new slot.
			// But before, check whether we already have the same texture set to a slot.
			float textureIndex = 0;
			for (int i = 0; i < g_pRenderData2D->m_TextureSlotIndex; i++) {

				// Compare if tex are same.
				// We compare the assigned GPU index.
				// For now its the textures GUID, thus we can assure that we compare correctly!
				if (texture->GetSlot() == g_pRenderData2D->m_TextureSlots[i]->GetSlot()) {

					textureIndex = (float)i;
					break;
				}

			}


			// Check whether this is a new texture,
			// and if we reached the batches max textures count.
			if (textureIndex == 0) {


				// Start new batch with new texture.
				if (g_pRenderData2D->m_TextureSlotIndex >= g_pRenderData2D->maxTextures) {

					_nextBatch();
				}

				// Set the texture for the new batch.
				textureIndex = (float)g_pRenderData2D->m_TextureSlotIndex;
				g_pRenderData2D->m_TextureSlots[g_pRenderData2D->m_TextureSlotIndex] = texture;
				g_pRenderData2D->m_TextureSlotIndex += 1;
			}



			// Set the data for the current quadVertex from the memory protocol of the drawing entity.
			for (int i = 0; i < vertexCount; i++) {

				glm::vec4 temp = model_transform * glm::vec4(g_pRenderData2D->m_QuadVertexPositions[i], 1.0f);
				glm::vec3 position; position.x = temp.x; position.y = temp.y; position.z = temp.z;

				g_pRenderData2D->m_QuadVertexEnd->Position = position;

				g_pRenderData2D->m_QuadVertexEnd->Color = color;


				g_pRenderData2D->m_QuadVertexEnd->TextureIndex = textureIndex;


				g_pRenderData2D->m_QuadVertexEnd->TextureCoords = memoryProtocol->m_TextureCoords[i];

				/*
				cout << "Quadvertex Data: \n";
				cout << "Position:"  << g_pRenderData2D->m_QuadVertexEnd->Position.x << g_pRenderData2D->m_QuadVertexEnd->Position.y << g_pRenderData2D->m_QuadVertexEnd->Position.z << endl;
				cout << "Color: " << g_pRenderData2D->m_QuadVertexEnd->Color.r << g_pRenderData2D->m_QuadVertexEnd->Color.g << g_pRenderData2D->m_QuadVertexEnd->Color.b << g_pRenderData2D->m_QuadVertexEnd->Color .a <<endl;
				cout << "Texture Index/Slot: " << g_pRenderData2D->m_QuadVertexEnd->TextureIndex << endl;
				cout << "Texture Coords: " << g_pRenderData2D->m_QuadVertexEnd->TextureCoords.x << g_pRenderData2D->m_QuadVertexEnd->TextureCoords.y  << endl;
				*/

				// go to the next quad vertex.
				g_pRenderData2D->m_QuadVertexEnd++;
			}

			// Increase the index count.
			// For each set of quad vertices we have 6 indices.
			g_pRenderData2D->m_QuadIndexCount += 6;

		}




		void BatchRenderer2D::init() {


			// First, create out vertex array.
			g_pRenderData2D->m_BatchVertexArray = new QuadVertexArray();


			// Create the vertex buffer.
			g_pRenderData2D->m_BatchVertexBuffer = new QuadVertexBuffer(g_pRenderData2D->maxVerts * sizeof(QuadVertex));


			// Add the main batch buffer to vertex array.
			// In this function we set out default vertex buffer layout.
			// See Vertex Array..
			g_pRenderData2D->m_BatchVertexArray->addVertexBuffer(g_pRenderData2D->m_BatchVertexBuffer);



			// Init Quadvertices...
			const int verts_count = g_pRenderData2D->maxVerts;
			g_pRenderData2D->m_QuadVertexBegin = new QuadVertex[verts_count]; // These are not initialized.


			// Init indices...
			// Our indices are opengl unsigned int.
			const int index_count = g_pRenderData2D->maxIndices;

			//GLuint* indices = new GLuint[g_pRenderData2D->maxIndices];
			std::vector<GLuint> indices;
			indices.resize(g_pRenderData2D->maxIndices);

			/*
			Our indices are like:

						0, 1, 3, // first triangle
						1, 2, 3  // second triangle
			*/
			GLuint offset = 0;
			for (GLuint i = 0; i < index_count; i += 6) {

				// Set the indices data.

				indices[i + 0] = offset + 0; // First triangle...
				indices[i + 1] = offset + 1; //
				indices[i + 2] = offset + 2; //

				indices[i + 3] = offset + 2; // Second triangle...
				indices[i + 4] = offset + 3; //
				indices[i + 5] = offset + 0; //

				offset += 4;
			}


			// Now set the index buffers data..
			// And store it in the vertex array for drawing.
			g_pRenderData2D->m_BatchVertexArray->setIndexBuffer(new QuadIndexBuffer(&indices[0], index_count));
			indices.clear();


			// Set the default texture.
			g_pRenderData2D->m_WhiteTexture = new nautilus::graphics::ComponentTexture2D(); // Set the default texture.
			g_pRenderData2D->m_WhiteTexture->init("particle_texture_sixstar.png"); // load texture.


			// Set the textures for teh vertex array.
			GLint* samplers = new GLint[32];

			for (GLint i = 0; i < 32; i++) {
				samplers[i] = i;
			}


			// Initialize the shader.
			g_pRenderData2D->m_BatchShader = new nautilus::graphics::ComponentShader();
			g_pRenderData2D->m_BatchShader->init("shaderTest"); // Load vert and frag
			g_pRenderData2D->m_BatchShader->Use();


			// Now send the texture samplers to frag shader..
			// On render we fill the sampler pointers with actuall texture data...
			// SetUniformArray expects a "const GLint* values",
			// thus we have to give him a "const", else we only send ONE TEXTURE at index 0
			// to the Shader Program, that is false and will draw only one texture...
			g_pRenderData2D->m_BatchShader->SetUniformArray("u_Textures", samplers, 32);


			g_pRenderData2D->m_TextureSlots[0] = g_pRenderData2D->m_WhiteTexture; // Default texture.
		}


		void BatchRenderer2D::shutDown() {

			// Delete quad vertices.
			delete[] g_pRenderData2D->m_QuadVertexBegin;
		}


		// Functions must be called on begin of each "scene",
		// means before we call draw functions, we call beginScene, and as we are done,
		// we call endScene.
		void BatchRenderer2D::beginScene(glm::mat4 view_projection) {

			g_pRenderData2D->m_BatchShader->Use(); // Bind shader.

			g_pRenderData2D->m_BatchShader->SetUniform("u_ViewProjection", view_projection); // Upload matrix to gpu

			// Start buffer...
			_startBatch();
		}



		void BatchRenderer2D::endScene() {

			// Order to render to the screen.
			_flush();
		}


		void BatchRenderer2D::_startBatch() {


			// Reset data for batch.
			g_pRenderData2D->m_QuadIndexCount = 0;
			g_pRenderData2D->m_QuadVertexEnd = g_pRenderData2D->m_QuadVertexBegin;

			g_pRenderData2D->m_TextureSlotIndex = 1;
		}


		void BatchRenderer2D::_nextBatch() {

			_flush();
			_nextBatch();
		}



		void BatchRenderer2D::_flush() {

			using namespace std;


			if (g_pRenderData2D->m_QuadIndexCount == 0) return; // Nothing to draw.



			unsigned int datasize = (unsigned int)((BYTE*)g_pRenderData2D->m_QuadVertexEnd - (BYTE*)g_pRenderData2D->m_QuadVertexBegin);

			//cout << color(colors::BLUE);
			//cout << "Datasize: " << datasize << white << endl;



			// Set the actuall data for rendering to the batch buffer.
			g_pRenderData2D->m_BatchVertexBuffer->setBufferData(g_pRenderData2D->m_QuadVertexBegin, datasize);




			// Bind needed textures.
			// We start from index 0 in order to bind our standard white texture.. 
			for (int i = 0; i < g_pRenderData2D->m_TextureSlotIndex; i++) {

				/*
				cout << color(colors::GREEN);
				cout << "Setting uniform samler: \n";
				cout << "Slot [" << i << "]\n";
				cout << "Texture \"" << g_pRenderData2D->m_TextureSlots[i]->m_FilePath << "\" \n" << white;
				*/

				g_pRenderData2D->m_TextureSlots[i]->BindForBatch((GLuint)i); // Bind texture to certain slot.
			}





			// Now make a render call.
			// Render with data we have set and textures...

			// Set count of indices we have.
			// If we want draw all indices, we just set 0.
			// Else we want to draw the specified indices count.
			int count = g_pRenderData2D->m_QuadIndexCount ? g_pRenderData2D->m_QuadIndexCount : g_pRenderData2D->m_BatchVertexArray->getIndicesCount();

			//cout << color(colors::BLUE);
			//cout << "Indices Count: " << count << white << endl;


			glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, nullptr);



			glBindTexture(GL_TEXTURE_2D, 0); // Unbind textzres.
		}




	}



}
