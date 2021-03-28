#pragma once
#ifndef FMODAUDIOSYSTEM_H
#define FMODAUDIOSYSTEM_H

#include"SoundSystem.h"


/*
FMOD MODES: --> "behaiviour" of sound.
Any combinations through OR operator possible.

FMOD_DEFAULT --> combination of FMOD_LOOP_OFF, FMOD_HARDWARE, FMOD_2D.
FMOD_LOOP_OFF --> sound playes once.
FMOD_LOOP_NORMAL --> sound cycles.
FMOD_HARDWARE --> sound is mixed in hardware (+performance).
FMOD_SOFTWARE --> .. mixed in software (+features).
FMOD_2D --> 2 dimensional sound.
FMOD_3D --> 3 dimensional sound.
*/

#define CUSTOM_REVERB_PROPERTY {2500, 15, 45, 5000, 50, 75, 78, 250, -20, 250, 25, 20.0f}

namespace nautilus {

	namespace audio {

		typedef std::map<std::string, FMOD::Sound*> SoundMap;
		typedef std::map<FMOD::Sound*, FMOD::Channel*> ChannelMap;
		typedef std::map<std::string, FMOD::ChannelGroup*> ChannelGroupMap;
		typedef std::map<std::string, FMOD::Reverb3D*> ReverbMap;
		typedef std::map<FMOD::Channel*, FMOD::DSP*> DSPMap;



		// Probably should be made Singleton
		class FMODAudioSystem : public IAudioSystem
		{
		public:
			FMODAudioSystem() {};
			~FMODAudioSystem() { pFMODSystem->release(); };

			static FMODAudioSystem* get() {
				if (!g_pAudioSystem) g_pAudioSystem = new FMODAudioSystem();
				return g_pAudioSystem;
			}

			bool startUp() override;
			void shutDown() override;

			FMOD::System* GetFMODSystem() override { return pFMODSystem; };

			// Load sound and return pointer to it
			FMOD::Sound* LoadSound(std::string soundName, unsigned int mode) override {
				return _loadSound(soundName, mode);
			}


			FMOD::Sound* StreamSound(std::string soundName, unsigned int mode) override {
				return _streamSound(soundName, mode);
			}


			// Play sound immediately on default channel.
			FMOD::Channel* PlaySound(FMOD::Sound& sound) override {
				return _playSound(sound);
			}

			// Play sound immediately on default channel from SoundMap
			FMOD::Channel* PlaySound(std::string soundName) override {
				return _playSound(soundName);
			}

			// Pauses given sound.
			FMOD_RESULT PauseSound(FMOD::Channel* channel) override {
				return _pauseSound(channel);
			}

			FMOD_RESULT PauseSound(int channelID) override {
				return _pauseSound(channelID);
			}

			// Set volume of sound: Volume = [0, 1].
			FMOD_RESULT SetVolume(FMOD::Channel* channel, float vol) override {
				return _setVolume(channel, vol);
			}


			// Called every App-Update-Step
			void Update() override {
				pFMODSystem->update();
			}


			ChannelGroupMap* GetChannelGroupMap() {
				return(&m_mChannelGroupMap);
			}

			SoundMap* GetSoundMap() {
				return(&m_mSoundMap);
			}

			ChannelMap* GetChannelMap() {
				return(&m_mChannelMap);
			}

			/*
			Function mixes two reverb properties to one. (alpha stage)
			Example Usage:

			FMOD_REVERB_PROPERTIES p1 = FMOD_PRESET_ARENA;
			FMOD_REVERB_PROPERTIES p2 = FMOD_PRESET_CONCERTHALL;
			FMOD_REVERB_PROPERTIES my_prop = pFMODAudioSystem->MixReverbProperties(p1, p2);
			*/
			FMOD_REVERB_PROPERTIES& MixReverbProperties(FMOD_REVERB_PROPERTIES& X, FMOD_REVERB_PROPERTIES& Y) {
				return(*this->_combineFMOD_REVERB_PROPERTIES(X, Y));
			}

			// Creates DSP Effect and applies it to given channel.
			bool CreateDSP(FMOD_DSP_TYPE type, FMOD::DSP* dsp, FMOD::Channel& channel) {
				return(this->_createDSP(type, dsp, channel));
			}

			// Removes DSP from DSPMap, does not delete the DSP Object.
			void RemoveDSP(FMOD::Channel& channel) {
				this->_removeDSP(channel);
			}

		private:
			static FMODAudioSystem* g_pAudioSystem;
			FMOD::System* pFMODSystem = 0;
			SoundMap m_mSoundMap;
			ChannelMap m_mChannelMap;
			ChannelGroupMap m_mChannelGroupMap;

			ReverbMap m_mReverbMap;
			unsigned short m_sReverbInstances = 0; // FMOD allows a max of 4.

			DSPMap m_mDSPMap;


		private:
			FMOD_RESULT _setVolume(FMOD::Channel* channel, float vol);

			FMOD_RESULT _pauseSound(int channelID);

			FMOD_RESULT _pauseSound(FMOD::Channel* channel);

			FMOD::Channel* _playSound(std::string soundName);

			FMOD::Channel* _playSound(FMOD::Sound& sound);

			FMOD::Sound* _streamSound(std::string soundName, unsigned int mode);

			FMOD::Sound* _loadSound(std::string soundName, unsigned int mode);


			// Returns channel from associated sound
			FMOD::Channel* _getChannel(FMOD::Sound& associatedSound);

			// Returns channel from associated sound name
			FMOD::Channel* _getChannel(std::string soundName);


			// Create Grouping of Channels for simultanious controll of Sounds.
			bool _makeChannelGroup(std::string name);


			// Add Channel to a Grouping.
			bool _addToChannelGroup(FMOD::Channel& channel, std::string groupName);


			// Remove a Grouping from our map (will not be deleted).
			void _removeChannelGroup(std::string groupName);


			// Remove a Channel from specified ChannelGrouping (will not be deleted).
			void _removeChannelFromGroup(std::string groupName, FMOD::Channel& channel);


			// Create Reverberation-Effect and store in our ReverbMap.
			bool _createReverb3D(FMOD_VECTOR& positionVec, float minDist, float maxDist, FMOD_REVERB_PROPERTIES& props, std::string reverbName);


			// Activates given reverb.
			void _activateReverb(std::string name);


			// Deactivates given reverb.
			void _deactivateReverb(std::string name);


			FMOD_REVERB_PROPERTIES* _combineFMOD_REVERB_PROPERTIES(FMOD_REVERB_PROPERTIES& X, FMOD_REVERB_PROPERTIES& Y);


			// Creates DSP instance of given tyoe and applies to given channel. 
			// TODO: Make removal of DSP effect possible --> how would the user prefer to manage his DSP effects? By naming each?
			bool _createDSP(FMOD_DSP_TYPE type, FMOD::DSP* dsp, FMOD::Channel& channel);



			// Removes the DSP from the DSPMap. Beware! We do not delete it!
			void _removeDSP(FMOD::Channel& channel);


		};
	}
}

#endif // !AUDIOIMPLEMENTATION_H