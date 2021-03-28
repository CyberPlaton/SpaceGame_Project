#pragma once

#include"Base.h"
#include"ISoundSystem.h"


namespace nautilus {

	namespace audio {


		class ReverbZone {
		public:
			ReverbZone(ISoundSystem* system, FMOD_REVERB_PROPERTIES props, glm::vec3 position, float minDistance, float maxDistance);
			~ReverbZone();

			void setActive();
			void setInactive();


		private:

			Scope<FMOD::Reverb3D> m_ReverbData;

		private:

		};


	}



}
