#pragma once

#include"Base.h"
#include"EventSystem.h"

namespace nautilus {

	namespace audio {

		// Utility functions.
		FMOD_VECTOR toFMODVec(glm::vec3 vec);
		glm::vec3 toGLMVec3(FMOD_VECTOR vec);


		enum class PlayOptions {
			Loop = 0,
			Loop_Off
		};


		class ISoundSystem {
		public:

			virtual bool init() = 0;
			
			virtual bool loadSound(std::string, std::string, PlayOptions, bool) = 0;
			virtual bool loadSoundEvent(std::string, std::string, std::string) = 0;

			virtual void playSound(std::string) = 0;
			virtual void playSoundEvent(std::string, std::string) = 0;
			
			virtual void pauseSound(std::string) = 0;

			virtual void unpauseSound(std::string) = 0;

			virtual void stopSound(std::string) = 0;
			virtual void stopSoundEvent(std::string, std::string, bool) = 0;

			virtual bool isSoundPlayed(std::string) = 0;
			virtual bool isSoundEventPlayed(std::string, std::string) = 0;

			virtual void update() = 0;

			virtual FMOD::System* getFMODSystem() const = 0;
			virtual FMOD::Studio::System* getFMODStudioSystem() const = 0;


			virtual void setSoundVolume(std::string, float) = 0;
			virtual bool isSound2D(std::string) = 0;

			virtual void setSound3DPosition(std::string, glm::vec3) = 0;
			virtual void incrementSound3DPosition(std::string, glm::vec3) = 0;
		};

	}


}
