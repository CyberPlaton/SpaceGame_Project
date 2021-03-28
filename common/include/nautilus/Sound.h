#pragma once

#include"Base.h"
#include"ISoundSystem.h"


namespace nautilus {

	namespace audio {

		class Sound {
		public:

			// Load default 2D- Sound data.
			Sound(ISoundSystem* system, std::string filename, PlayOptions op, bool sound2D = true);
			~Sound();

			
			
			// Play loaded audio data.
			// Does not restart or stop if sound is already playing.
			void play(ISoundSystem* system);


			// Immediately stop playing.
			void stop();


			void pause();
			void unpause(); // Should only be called if we paused the sound (and not stopped).
		

			bool isPlaying() const { return m_IsPlaying; }
			bool is2D() const { FMOD_MODE* mode = NULL; return m_SoundData->getMode(mode) == FMOD_2D; }

			void set3DPosition(glm::vec3 pos);
			void increment3DPosition(glm::vec3 vec);
			void setVolume(float vol);

			FMOD::Sound* getData() const { return m_SoundData.get(); }

		private:

			std::string m_Filepath;
			bool m_IsPlaying = false;

			Scope<FMOD::Sound> m_SoundData;


			// Every sound plays on own channel.
			Scope<FMOD::Channel> m_SoundChannel;
		private:

		};

	}

}
