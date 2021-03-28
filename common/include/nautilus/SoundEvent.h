#pragma once

#include"Base.h"
#include"ISoundSystem.h"


namespace nautilus {

	namespace audio {

		class SoundEvent {
		public:
			SoundEvent(ISoundSystem* system, std::string bankFilename, std::string stringBankFilename);
			
			// Loads an event specified in the bank file.
			// Give "eventName" as e.g. "Ambience", it will be coverted to
			// "event:/Ambience" automatically. Further refer to this event as "Ambience" too...
			bool loadEvent(ISoundSystem* system, std::string eventName);

			// Play a certain event audio data.
			void playEvent(std::string eventName);

			// Check whether a certain event is currently played.
			bool isPlaying(std::string eventName) const;

			// Function set the sound to stop.
			// How this stop will be executed is defined in FMOD Studio,
			// whether abruptly or with fadeout or maybe some bank events...
			//
			// Furthermore, this will release the event instance from sound event.
			void stopEvent(std::string eventName, bool fadeOut = true);


		private:


			std::map<std::string, Scope<FMOD::Studio::EventInstance> > m_SoundEventDataMap;

		private:

		};

	}


}
