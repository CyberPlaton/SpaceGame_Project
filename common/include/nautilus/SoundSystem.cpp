#include"SoundSystem.h"

namespace nautilus {

	namespace audio {


		void SoundSystem::destroyReverbZone(std::string reverbName) {

			for (auto it = m_ReverbZoneMap.begin(); it != m_ReverbZoneMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, reverbName) == 0) {
					m_ReverbZoneMap.erase(it);
					return;
				}
			}

			using namespace std;
			cout << color(colors::RED);
			cout << "Reverb zone to be deleted was not found  \"" << reverbName << "\"." << white << endl;
		}


		bool SoundSystem::createReverbZone(FMOD_REVERB_PROPERTIES props, glm::vec3 position, float minDistance, float maxDistance, std::string reverbZoneName){

			// Check if zone name already exists.
			if (m_ReverbZoneMap.find(reverbZoneName) != m_ReverbZoneMap.end()) return false;


			ReverbZone* zone = new ReverbZone(this, props, position, minDistance, maxDistance);

			if (zone) {

				// Save reference to reverb zone we have created.
				m_ReverbZoneMap.emplace(reverbZoneName, Scope<ReverbZone>(zone));

				return true;
			}


			using namespace std;
			cout << color(colors::RED);
			cout << "Error on creating reverb zone \"" << reverbZoneName << "\"." << white << endl;

			return false;
		}


		void SoundSystem::activateReverbZone(std::string reverbName) {

			for (auto it = m_ReverbZoneMap.begin(); it != m_ReverbZoneMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, reverbName) == 0) {

					it->second->setActive();
					return;
				}
			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Reverb Zone for activation not found: \"" << reverbName << "\"." << white << endl;
		}


		void SoundSystem::deactivateReverbZone(std::string reverbName) {

			for (auto it = m_ReverbZoneMap.begin(); it != m_ReverbZoneMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, reverbName) == 0) {

					it->second->setInactive();
					return;
				}
			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Reverb Zone for deactivation not found: \"" << reverbName << "\"." << white << endl;
		}


		bool SoundSystem::init() {

			FMOD_RESULT result;

			FMOD::Studio::System* studio = nullptr;
			FMOD::System* fmod = nullptr;


			// Create and init fmod studio api.
			result = FMOD::Studio::System::create(&studio);
			if (result != FMOD_OK && "Could not create FMOD Studio!") return false;

			result = studio->getCoreSystem(&fmod);
			if (result != FMOD_OK && "Could not initialize FMOD Core!") return false;

			result = fmod->setSoftwareFormat(0, FMOD_SPEAKERMODE_DEFAULT, 0);
			if (result != FMOD_OK && "Could not set software Format for FMOD Core!") return false;


			result = studio->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
			if (result != FMOD_OK && "Could not initialize FMOD Audio Systems!") return false;




			m_FMODSystem = Scope< FMOD::System >(fmod);
			m_FMODStudioSystem = Scope< FMOD::Studio::System >(studio);


			// Initialize 3D attributes.
			// Default Listener is at 0, 0.
			FMOD_VECTOR  listenerpos = { 0.0f, 0.0f, -1.0f };
			FMOD_VECTOR forward = {0.0f, 0.0f, 1.0f};
			FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
			m_FMODSystem->set3DListenerAttributes(0, &listenerpos, 0, &forward, &up);

			return true;
		}


		bool SoundSystem::loadSound(std::string filename, std::string soundName, PlayOptions op, bool sound2D) {

			// Check whether sound already loaded.
			if (m_SoundMap.find(soundName) != m_SoundMap.end()) return true;



			// Load sound file.
			Sound* sound = nullptr;
			sound = new Sound(this, filename, op, sound2D);

			if (sound) {

				m_SoundMap.emplace(soundName, Scope<Sound>(sound));
				return true;
			}
			else {
				using namespace std;
				cout << color(colors::RED);
				cout << "Could not load Sound file \""<< filename << "\"." << white << endl;
				
				return false;
			}
		}


		bool SoundSystem::loadSoundEvent(std::string bankFilename, std::string stringBankFilename, std::string soundName) {

			// Check whether sound event already loaded.
			if (m_SoundEventMap.find(soundName) != m_SoundEventMap.end()) return true;

			// Load sound event.
			SoundEvent* sound = nullptr;
			sound = new SoundEvent(this, bankFilename, stringBankFilename);
			
			if (sound) {

				// Store to map.
				m_SoundEventMap.emplace(soundName, Scope<SoundEvent>(sound));
				return true;
			}
			else {
				using namespace std;
				cout << color(colors::RED);
				cout << "Could not load Sound Event file \"" << bankFilename << "\"." << white << endl;

				return false;
			}
		}


		void SoundSystem::playSound(std::string sound) {

			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					it->second->play(this);
					return;
				}
			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for playing not found: \""<< sound << "\"." << white << endl;
		}



		void SoundSystem::playSoundEvent(std::string sound, std::string eventName) {

			for (auto it = m_SoundEventMap.begin(); it != m_SoundEventMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) it->second->playEvent(eventName);
				return;
			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound Event for playing not found: \"" << sound << "\"." << white << endl;
		}



		FMOD::System* SoundSystem::getFMODSystem() const {

			return m_FMODSystem.get();
		}



		FMOD::Studio::System* SoundSystem::getFMODStudioSystem() const {

			return m_FMODStudioSystem.get();
		}



		void SoundSystem::update() {

			if (!m_FMODStudioSystem) return;
			if (!m_FMODSystem) return;


			std::vector< std::map<std::string, Scope<Sound>>::iterator > unplayedSoundIndeces;
			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (!it->second->isPlaying()) {

					// Release handle to unplayed sound.
					it->second.release();

					// Store index of unplayed sound.
					unplayedSoundIndeces.push_back(it);
				}

			}

			// Delete unplayed sounds.
			for (auto it: unplayedSoundIndeces) {
				
				// Just deletes entry..
				m_SoundMap.erase(it);
			}




			// Let FMOD update its systems.
			m_FMODSystem->update();
			m_FMODStudioSystem->update();
		}



		void SoundSystem::pauseSound(std::string sound) {

			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					it->second->pause();
					return;
				}
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for pausing not found: \"" << sound << "\"." << white << endl;
		}




		bool SoundSystem::isSoundPlayed(std::string sound) {


			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) return it->second->isPlaying();
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound not found: \"" << sound << "\"." << white << endl;

			return false;
		}


		bool SoundSystem::isSoundEventPlayed(std::string sound, std::string eventName) {

			for (auto it = m_SoundEventMap.begin(); it != m_SoundEventMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) return it->second->isPlaying(eventName);
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound Event not found: \"" << sound << "\"." << white << endl;

			return false;
		}



		void SoundSystem::stopSound(std::string sound) {


			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {


					it->second->stop();
					return;
				}
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for stop not found: \"" << sound << "\"." << white << endl;
		}


		void SoundSystem::stopSoundEvent(std::string sound, std::string eventName, bool fadeOut) {


			for (auto it = m_SoundEventMap.begin(); it != m_SoundEventMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					it->second->stopEvent(eventName, fadeOut);
					return;
				}
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound Event for stop not found: \"" << sound << "\"." << white << endl;
		}




		void SoundSystem::unpauseSound(std::string sound) {

			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					it->second->unpause();
					return;
				}
				
			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for unpause not found: \"" << sound << "\"." << white << endl;
		}



		bool SoundSystem::loadBankEvent(std::string soundEventName, std::string bankEventName) {

			for (auto it = m_SoundEventMap.begin(); it != m_SoundEventMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, soundEventName) == 0) {

					return it->second->loadEvent(this, bankEventName);
				}
			}


			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound Event for loading BankEvent not found: Sound:\"" << soundEventName << "\",  Bank:\""<< bankEventName << "\"" << white << endl;

			return false;
		}


		void SoundSystem::setSoundVolume(std::string sound, float vol) {

			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					it->second->setVolume(vol);
					return;
				}

			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for volume setting not found: \"" << sound << "\"." << white << endl;
		}


		bool SoundSystem::isSound2D(std::string sound) {

			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					return it->second->is2D();
				}

			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for is2D  not found: \"" << sound << "\"." << white << endl;
		}


		void SoundSystem::setSound3DPosition(std::string sound, glm::vec3 pos) {

			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					it->second->set3DPosition(pos);
					return;
				}

			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for set 3D position  not found: \"" << sound << "\"." << white << endl;
		}


		void SoundSystem::incrementSound3DPosition(std::string sound, glm::vec3 vec) {

			for (auto it = m_SoundMap.begin(); it != m_SoundMap.end(); it++) {

				if (COMPARE_STRINGS_2(it->first, sound) == 0) {

					it->second->increment3DPosition(vec);
					return;
				}

			}

			using namespace std;
			cout << color(colors::DARKRED);
			cout << "Sound for increment 3D position  not found: \"" << sound << "\"." << white << endl;
		}
	}


}
