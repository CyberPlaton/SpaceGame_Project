#include"ReverbZone.h"

namespace nautilus {

	namespace audio {

		ReverbZone::ReverbZone(ISoundSystem* system, FMOD_REVERB_PROPERTIES props, glm::vec3 position, float minDistance, float maxDistance) {

			assert(system);
			assert(system->getFMODSystem());

			FMOD_RESULT result;
			FMOD::Reverb3D* reverb = nullptr;
			result = system->getFMODSystem()->createReverb3D(&reverb);

			if (result == FMOD_OK) {

				// Set reverb properties,
				// like concert hall...
				reverb->setProperties(&props);
				

				// Set the position definitions for reverb...
				FMOD_VECTOR vec = toFMODVec(position);
				reverb->set3DAttributes(&vec, minDistance, maxDistance);


				// Store reference to reverb.
				m_ReverbData = Scope<FMOD::Reverb3D>(reverb);

			}
			else {
				delete reverb;
				return;
			}
		}
		
		
		ReverbZone::~ReverbZone() {

			m_ReverbData->release();
			m_ReverbData.release();

		}

		
		void ReverbZone::setActive() {


			m_ReverbData->setActive(true);
		}
		
		
		void ReverbZone::setInactive() {


			m_ReverbData->setActive(false);
		}

	}

}
