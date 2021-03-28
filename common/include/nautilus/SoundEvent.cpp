#include"SoundEvent.h"

namespace nautilus {

	namespace audio {


		SoundEvent::SoundEvent(ISoundSystem* system, std::string bankFilename, std::string stringBankFilename) {

			assert(system);
			assert(system->getFMODStudioSystem());


			FMOD_RESULT result_bank, result_bankstring;

			FMOD::Studio::Bank* bankFile = nullptr;
			FMOD::Studio::Bank* stringBankFile = nullptr;


			result_bank = system->getFMODStudioSystem()->loadBankFile(bankFilename.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bankFile);
			result_bankstring = system->getFMODStudioSystem()->loadBankFile(stringBankFilename.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringBankFile);


			if (result_bank == FMOD_OK && result_bankstring == FMOD_OK) {
				using namespace std;


				cout << color(colors::GREEN);
				cout << "Successfuly loading Bankfile: \"" << bankFilename << "\"." << white << endl;
			}
			else {
				using namespace std;

				cout << color(colors::RED);
				cout << "Failed loading Bankfile: \""<< bankFilename << "\"." << white << endl;
			}

		}



		// Play a certain event audio data.
		void SoundEvent::playEvent(std::string eventName) {

			for (auto it = m_SoundEventDataMap.begin(); it != m_SoundEventDataMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, eventName) == 0) {

					it->second->start();
					return;
				}
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Bank Event for starting not found: \"" << eventName << "\"" << white << endl;
		}



		void SoundEvent::stopEvent(std::string eventName, bool fadeOut) {


			for (auto it = m_SoundEventDataMap.begin(); it != m_SoundEventDataMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, eventName) == 0) {


					FMOD_STUDIO_STOP_MODE mode = (fadeOut == true) ? FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_IMMEDIATE;

					it->second->stop(mode);
					it->second->release();
					it->second.release();
					m_SoundEventDataMap.erase(it);
					return;
				}
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Bank Event for stopping not found:\"" << eventName << "\"" << white << endl;

		}



		bool SoundEvent::isPlaying(std::string eventName) const {

			for (auto it = m_SoundEventDataMap.begin(); it != m_SoundEventDataMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, eventName) == 0) {

					FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
					return it->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING;
				}
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Bank Event not found: \"" << eventName << "\"" << white << endl;
			return false;
		}



		bool SoundEvent::loadEvent(ISoundSystem* system, std::string eventName) {

			FMOD_RESULT result;

			FMOD::Studio::EventDescription* desc = nullptr;
			FMOD::Studio::EventInstance* inst = nullptr;

			std::string eName = "event:/" + eventName;

			result = system->getFMODStudioSystem()->getEvent(eName.c_str(), &desc);
			if (result != FMOD_OK) {
				return false;
			}


			result = desc->createInstance(&inst);
			if (result != FMOD_OK) {
				return false;
			}

			// Save event instance to map.
			if (inst) m_SoundEventDataMap.emplace(eventName, Scope<FMOD::Studio::EventInstance>(inst));

			return true;
		}
	}

}
