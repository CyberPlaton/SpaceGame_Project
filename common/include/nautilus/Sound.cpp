#include"Sound.h"

namespace nautilus {

	namespace audio {

		Sound::~Sound() {

			m_SoundData.reset();
		}


		void Sound::set3DPosition(glm::vec3 pos) {

			FMOD_VECTOR vec = toFMODVec(pos);

			if (!is2D()) {
				m_SoundChannel->set3DAttributes(&vec, 0);
			}
		}


		void Sound::increment3DPosition(glm::vec3 vec) {

			glm::vec3 pos;
			FMOD_VECTOR fmodPos;
			m_SoundChannel->get3DAttributes(&fmodPos, 0);
			pos = toGLMVec3(fmodPos);
			pos += vec;

			fmodPos = toFMODVec(pos);

			if (!is2D()) {
				m_SoundChannel->set3DAttributes(&fmodPos, 0);
			}
		}


		void Sound::setVolume(float vol) {

			if (vol >= 1.0f) vol = 1.0f;
			if (vol <= 0.0f) vol = 0.0f;

			m_SoundChannel->setVolume(vol);
		}


		Sound::Sound(ISoundSystem* system, std::string filename, PlayOptions op, bool sound2D) {

			assert(system);
			assert(system->getFMODSystem());

			// Set the given mode.
			// Set whether sound is 2D or not.
			FMOD_MODE mode = (op == PlayOptions::Loop) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
			mode |= (sound2D) ? FMOD_2D : FMOD_3D;
			
			FMOD::Sound* sound = nullptr;
			FMOD_RESULT result = system->getFMODSystem()->createSound(filename.c_str(), mode, 0, &sound);

			if (result == FMOD_OK) {

				// Create reference to sound data...
				m_SoundData = Scope<FMOD::Sound>(sound);

				m_Filepath = filename;
			}
			else {
				using namespace std;

				cout << color(colors::RED);
				cout << "Failed to Load Sound data: \"" << filename << "\"." << white << endl;
			}
		}



		// Play loaded audio data.
		void Sound::play(ISoundSystem* system) {

			// Is already playing?
			if (m_IsPlaying) return;

			
			if (m_SoundChannel) {
				m_SoundChannel.release(); // Release does not delete previous SoundChannel, only drops ownership.
										  // Thus SoundChannel will be automatically deallocated, when ref count is 0.
			}
			


			FMOD_RESULT result;

			FMOD::Channel* channel = nullptr;

			result = system->getFMODSystem()->playSound(m_SoundData.get(), NULL, false, &channel);

			if (result == FMOD_OK) {


				m_SoundChannel = Scope<FMOD::Channel>(channel);

				m_IsPlaying = true;
			}
			else {
				using namespace std;

				cout << color(colors::RED);
				cout << "Failed to Play Sound data: \"" << m_Filepath << "\"." << white << endl;
			}
		}



		void Sound::unpause() {

			if (m_IsPlaying) return;

			m_SoundChannel->setPaused(false);

			m_IsPlaying = true;

		}



		// Immediately stop playing.
		void Sound::stop() {

			if (!m_IsPlaying) return;

			m_SoundChannel->stop();

			m_IsPlaying = false;
		}


		void Sound::pause() {

			if (!m_IsPlaying) return;

			m_SoundChannel->setPaused(true);

			m_IsPlaying = false;
		}
	}

}
