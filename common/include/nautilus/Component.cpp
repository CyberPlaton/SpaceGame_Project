#include"Component.h"

#define STB_IMAGE_IMPLEMENTATION
#include"common/include/stb_image/stb_image.h"

namespace nautilus {

	namespace graphics {



        void ComponentTexture2D::init(std::string filename) {
            LoadTexture(filename);
            m_FilePath = filename;
        }


		ComponentTexture2D::~ComponentTexture2D() {


		}


		bool ComponentTexture2D::LoadTexture(const std::string& fileName, bool genMipMaps) {

			int w, h, cmp; // cmp = rgb or rgba etc.

			// Load image.
			unsigned char* imgData = stbi_load(fileName.c_str(), &w, &h, &cmp, STBI_rgb_alpha);

			if (imgData != NULL) {

				// Invrt image orientation.
				int byteWidth = w * 4;
				unsigned char* top = NULL;
				unsigned char* bottom = NULL;
				unsigned char temp = NULL;
				int halfHeight = h / 2;
				for (int row = 0; row < halfHeight; row++) {
					top = imgData + row * byteWidth;
					bottom = imgData + (h - row - 1) * byteWidth;
					for (int col = 0; col < byteWidth; col++) {
						temp = *top;
						*top = *bottom;
						*bottom = temp;
						top++;
						bottom++;
					}
				}


				// Get information from image.
				m_Size = glm::vec2(w, h); // Size aka Dimensions.
				m_FilePath = fileName; // Save path for resource manager.

				// Create texture object.
				glGenTextures(1, &m_TextureHandle);


				glBindTexture(GL_TEXTURE_2D, m_TextureHandle);


				// Apply options for texture.
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // Up axis for OpenGL texels
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Down axis.

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear texture filtering.
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// Map bits from image to opengl texture.
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

				// Generate mip maps.
				if (genMipMaps) {
					glGenerateMipmap(GL_TEXTURE_2D);
				}

				// Free memory of image.
				stbi_image_free(imgData);

				// Free binding for image = unbind. 
				glBindTexture(GL_TEXTURE_2D, 0);

				return true;
			}
			else {

				using namespace std;
				cout << color(colors::RED);
				cout << "Error Loading image: " << fileName << white << endl;
				return false;
			}
		}

		void ComponentTexture2D::Bind(GLuint texUint) {

            // To bind try too...
            // 	glBindTextureUnit(slot, m_RendererID);


			glActiveTexture(GL_TEXTURE0 + texUint); // For binding texture to "texUint" particular index.

			glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
		}



        void ComponentTexture2D::BindForBatch(GLuint texUint ) {

            glBindTextureUnit(texUint, m_TextureHandle);
        }


		void ComponentTexture2D::Unbind(GLuint texUint) {

			glActiveTexture(GL_TEXTURE0 + texUint); // Bind to textures index a 0 texture --> unbind.
			glBindTexture(GL_TEXTURE_2D, 0);
		}



        void  ComponentShader::init(std::string filename) {

            LoadShaders((filename + ".vert").c_str(), (filename + ".frag").c_str());
        }


        bool ComponentShader::LoadShaders(const char* vsFilename, const char* fsFilename) {

            using namespace std;

            // Load shaders as strings and as const char (for interoperability).
            string vsString = _fileToString(vsFilename);
            string fsString = _fileToString(fsFilename);
            const GLchar* vsPtr = vsString.c_str();
            const GLchar* fsPtr = fsString.c_str();



            // Shader creation for mesh...
            // Vertex shader...
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vs, 1, &vsPtr, NULL); // Assing source for shader.
            glCompileShader(vs); // Compile it...

            _compilingCheck(vs, ShaderType::SHADER_TYPE_VERTEXSHADER);




            // Fragment shader...
            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fs, 1, &fsPtr, NULL);
            glCompileShader(fs);

            _compilingCheck(fs, ShaderType::SHADER_TYPE_FRAGMENTSHADER);




            // Make shader programm..
            m_ProgramHandle = glCreateProgram();
            glAttachShader(m_ProgramHandle, vs); // Attach shader to program...
            glAttachShader(m_ProgramHandle, fs); // Attach shader to program...


            // Link program...
            glLinkProgram(m_ProgramHandle);

            _compilingCheck(m_ProgramHandle, ShaderType::SHADER_TYPE_SHADERPROGRAM);


            // As shaders are now in program, we can delete them... to avoid memory leak etc.
            glDeleteShader(vs);
            glDeleteShader(fs);

            m_FilePath = vsFilename;

            return true;
        }




        void ComponentShader::Use() {

            if (m_ProgramHandle > 0) glUseProgram(m_ProgramHandle);
        }



        std::string ComponentShader::_fileToString(const std::string& filename) {

            using namespace std;
            stringstream ss;
            ifstream file;

            try {

                file.open(filename, ios::in);

                if (!file.fail()) {


                    ss << file.rdbuf(); // Load content to string stream.
                }

                file.close();
            }
            catch (exception e) {
                cout << "Error: " << e.what() << endl;
            }

            return ss.str();
        }



        void ComponentShader::_compilingCheck(GLuint shaderObj, ShaderType sType) {

            using namespace std;

            int status = 0;

            if (sType == ShaderType::SHADER_TYPE_VERTEXSHADER || sType == ShaderType::SHADER_TYPE_FRAGMENTSHADER) {

                glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE) {

                    GLint length = 0;
                    glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &length);
                    string errorLog(length, ' ');
                    glGetShaderInfoLog(shaderObj, length, &length, &errorLog[0]);

                    cout << color(colors::RED);
                    cout << "Shader Compile Error! " << errorLog << white << endl;
                }
            }
            else {

                glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &status);
                if (status == GL_FALSE) {

                    GLint length = 0;
                    glGetProgramiv(m_ProgramHandle, GL_INFO_LOG_LENGTH, &length);
                    string errorLog(length, ' ');
                    glGetProgramInfoLog(m_ProgramHandle, length, &length, &errorLog[0]);

                    cout << color(colors::RED);
                    cout << "Link Error! " << errorLog << white << endl;
                }
            }
        }



        ComponentShader::~ComponentShader() {

            glDeleteProgram(m_ProgramHandle);
            m_UniformLocationsMap.clear();
        }



        GLint ComponentShader::_getUniformLocation(const GLchar* name) {

            std::map<std::string, GLint>::iterator it = m_UniformLocationsMap.find(name);

            if (it == m_UniformLocationsMap.end()) { // Nothing found.

                m_UniformLocationsMap[name] = glGetUniformLocation(m_ProgramHandle, name); // Set a location index.

            }


            return m_UniformLocationsMap[name]; // Return the location index.
        }


        void ComponentShader::SetUniform(const GLchar* name, const glm::vec2& v) {

            GLint location = _getUniformLocation(name); // Get location index

            glUniform2f(location, v.x, v.y); // Set values for the uniform.
        }



        void ComponentShader::SetUniform(const GLchar* name, const glm::vec3& v) {

            GLint location = _getUniformLocation(name); // Get location index

            glUniform3f(location, v.x, v.y, v.z); // Set values for the uniform.
        }


        void ComponentShader::SetUniform(const GLchar* name, const glm::vec4& v) {


            GLint location = _getUniformLocation(name); // Get location index

            glUniform4f(location, v.x, v.y, v.z, v.w); // Set values for the uniform.
        }


        void ComponentShader::SetUniform(const GLchar* name, const glm::mat4& m) {

            GLint location = _getUniformLocation(name); // Get location index

            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
        }


        void ComponentShader::SetUniform(const GLchar* name, const GLfloat& f) {

            GLint location = _getUniformLocation(name); // Get location index

            glUniform1f(location, f); // Set values for the uniform.
        }

        void ComponentShader::SetUniformSampler(const GLchar* name, const GLint& textureSlot) {

            glActiveTexture(GL_TEXTURE + textureSlot); // Activate texture.

            GLint location = _getUniformLocation(name); // Get location index

            glUniform1i(location, textureSlot); // Set values for the uniform.
        }



        void ComponentShader::SetUniform(const GLchar* name, const GLint& i) {

            GLint location = _getUniformLocation(name); // Get location index

            glUniform1i(location, i); // Set values for the uniform.
        }


        void ComponentShader::SetUniformArray(const GLchar* name, const GLint* values, int count) {

            GLint location = _getUniformLocation(name); // Get location index

            glUniform1iv(location, count, values);
        }
















	}



}
