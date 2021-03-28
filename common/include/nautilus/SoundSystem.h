#pragma once
#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include"Base.h"
#include"Sound.h"
#include"SoundEvent.h"
#include"ReverbZone.h"



namespace nautilus {

	namespace audio {

#define CUSTOM_REVERB_PROPERTY {2500, 15, 45, 5000, 50, 75, 78, 250, -20, 250, 25, 20.0f}

		class SoundSystem : public ISoundSystem {
		public:
			SoundSystem() = default;

			// Initialize fmod and fmod studio.
			bool init() override;

			// Load a sound for playing with specified options.
			bool loadSound(std::string filename, std::string soundName, PlayOptions op, bool sound2D = true) override;

			// Loading of bank file and associated strings file.
			// Does not load any event instances to sound event.
			// Thus, to play an event, first call "loadBankEvent" to load a event defined in the bank file.
			bool loadSoundEvent(std::string bankFilename, std::string stringBankFilename, std::string soundName) override;

			void playSound(std::string sound) override;
			void playSoundEvent(std::string sound, std::string eventName) override;

			// Stop sound from being played.
			// It will be freed on update and must be reloaded in order to play it again.
			void stopSound(std::string sound) override;
			void stopSoundEvent(std::string sound, std::string eventName, bool fadeOut = true) override;

			// Temporarily pause a sound.
			// To play it from where it was paused, use unpause.
			void pauseSound(std::string sound) override;
			void unpauseSound(std::string sound) override;

			// Check whether a sound is played.
			bool isSoundPlayed(std::string sound) override;

			// Check whether a bank file event from a sound event is played.
			bool isSoundEventPlayed(std::string sound, std::string eventName) override;

			// Function releases sounds that are not being played
			// and calls update functions of fmod and fmod studio.
			void update() override;


			// We do not force the FMOD system to be uncopyable..
			// But, if user saves a pointer to the instance,
			// he must make sure that that pointer is deallcated correctly.
			// So best,
			// use a pointer in functions and let them go out of scope.
			FMOD::System* getFMODSystem() const;
			FMOD::Studio::System* getFMODStudioSystem() const;


			void setSound3DPosition(std::string sound, glm::vec3 pos) override;
			void incrementSound3DPosition(std::string sound, glm::vec3 vec) override;


			void setSoundVolume(std::string sound, float vol) override;


			bool isSound2D(std::string sound) override;


			// Load am event from bank file to a sound event. This makes it ready to be played.
			bool loadBankEvent(std::string soundEventName, std::string bankEventName);


			bool createReverbZone(FMOD_REVERB_PROPERTIES props, glm::vec3 position, float minDistance, float maxDistance, std::string reverbZoneName);
			void activateReverbZone(std::string reverbName);
			void deactivateReverbZone(std::string reverbName);
			void destroyReverbZone(std::string reverbName);


		private:
			Scope<FMOD::System> m_FMODSystem;
			Scope<FMOD::Studio::System> m_FMODStudioSystem;


			std::map<std::string, Scope<Sound>> m_SoundMap;
			std::map<std::string, Scope<SoundEvent>> m_SoundEventMap;
			std::map<std::string, Scope<ReverbZone>> m_ReverbZoneMap;
			Scope<FMOD::Reverb3D> m_DefaultAmbientReverb;

			glm::vec3 m_ListenerPosition = { 0.0f, 5.0f, -10.0f };

			unsigned int m_ReverbCount = 0;
		private:

		};




		class IAudioSystem
		{
		public:
			virtual bool startUp() = 0;
			virtual void shutDown() = 0;

			virtual FMOD::System* GetFMODSystem() = 0;

			virtual FMOD::Sound* LoadSound(std::string soundName, unsigned int mode) = 0;
			virtual FMOD::Sound* StreamSound(std::string soundName, unsigned int mode) = 0;

			virtual FMOD::Channel* PlaySound(FMOD::Sound& sound) = 0;
			virtual FMOD::Channel* PlaySound(std::string soundName) = 0;

			virtual FMOD_RESULT PauseSound(FMOD::Channel* channel) = 0;
			virtual FMOD_RESULT PauseSound(int channelID) = 0;

			virtual FMOD_RESULT SetVolume(FMOD::Channel* channel, float vol) = 0;


			virtual void Update() = 0;

		private:
			FMOD::System* pFMODSystem = 0;
		};
	}

}


#endif // !SOUNDSYSTEM_H