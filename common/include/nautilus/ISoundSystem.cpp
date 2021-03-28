#include"ISoundSystem.h"

namespace nautilus {

	namespace audio {


		FMOD_VECTOR toFMODVec(glm::vec3 vec) {
			FMOD_VECTOR fVec;
			fVec.x = vec.x;
			fVec.y = vec.y;
			fVec.z = vec.z;

			return fVec;
		}

		glm::vec3 toGLMVec3(FMOD_VECTOR vec) {
			glm::vec3 glmVec;
			glmVec.x = vec.x;
			glmVec.y = vec.y;
			glmVec.z = vec.z;

			return glmVec;
		}

	}

}
